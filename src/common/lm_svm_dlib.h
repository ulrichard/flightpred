#ifndef LM_SVM_DLIB_H_INCLUDED
#define LM_SVM_DLIB_H_INCLUDED

// flightpred
#include "common/learning_machine.h"
#include "common/flightpred_globals.h"
#include "common/reporter.h"
// dlib
#include <dlib/svm.h>
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// boost
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/base_object.hpp>
// standard library
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <typeinfo>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
template<class kernel_type>
class lm_dlib_base : public learning_machine
{
    friend class boost::serialization::access;
protected:
    lm_dlib_base(const std::string &pred_name)
      : learning_machine(pred_name) { }

public:
    typedef typename kernel_type::sample_type sample_type;

    virtual ~lm_dlib_base() { };

    virtual void train(const learning_machine::SampleType &samplesin, const std::vector<double> &lables)
    {
        assert(samplesin.size() == lables.size());

        // format transformation
        std::vector<sample_type> samples;
        for(learning_machine::SampleType::const_iterator it = samplesin.begin(); it != samplesin.end(); ++it)
        {
            sample_type samp;
            samp.set_size(it->size());
            for(size_t i=0; i<it->size(); ++i)
                samp(i) = (*it)[i];
            samples.push_back(samp);
        }

        reporting::report(reporting::VERBOSE) << "normalize the samples";
        this->normalizer_.train(samples);
        std::transform(samples.begin(), samples.end(), samples.begin(), this->normalizer_);

        train_algorithm(samples, lables);
    }

    virtual double eval(const std::vector<double> &sample) const
    {
        // format conversion
        sample_type samp;
        samp.set_size(sample.size());
        for(size_t i=0; i<sample.size(); ++i)
            samp(i) = sample[i];

        // normalize the samples
        samp = normalizer_(samp);

        // evaluate
        return learnedfunc_(samp);
    }

    virtual void write_to_db(const size_t conf_id)
    {
        pqxx::transaction<> trans(flightpred_db::get_conn(), "write svm to db");

        size_t oid_blob = 0;
        {
            pqxx::largeobject dblobj(trans);
            pqxx::olostream dbstrm(trans, dblobj);
            reporting::report(reporting::VERBOSE) << "streaming normalizer and SVM to the db blob";
            dlib::serialize(normalizer_, dbstrm);
            dlib::serialize(learnedfunc_, dbstrm);
            oid_blob = dblobj.id();
        }

        std::stringstream sstr;
        sstr << "UPDATE trained_solutions SET " << pred_name_ << "=" << oid_blob << " WHERE train_sol_id=" << conf_id;
        trans.exec(sstr.str());
        trans.commit();

        reporting::report(reporting::VERBOSE) << pred_name_ << " (" << oid_blob << ") has " << learnedfunc_.basis_vectors.nr() << " support vectors.";
    }

    virtual void read_from_db(const size_t conf_id)
    {
        pqxx::transaction<> trans(flightpred_db::get_conn(), "read svm from db");

        std::stringstream sstr;
        sstr << "SELECT " << pred_name_ << " FROM trained_solutions WHERE train_sol_id=" << conf_id;
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            throw std::runtime_error("solution configuration not found");
        if(res[0][0].is_null())
            return;
        size_t oid_blob;
        res[0][0].to(oid_blob);

        pqxx::ilostream dbstrm(trans, oid_blob);
        assert(dbstrm.good());
        dlib::deserialize(normalizer_, dbstrm);
        dlib::deserialize(learnedfunc_, dbstrm);

        reporting::report(reporting::VERBOSE) << pred_name_ << " (" << oid_blob << ") has " << learnedfunc_.basis_vectors.nr() << " support vectors.";
    }

    virtual int main_metric() const
    {
        return learnedfunc_.basis_vectors.nr();
    }

    virtual std::string description() const
    {
        const std::string classnam(typeid(*this).name());
        return classnam + boost::lexical_cast<std::string>(learnedfunc_.basis_vectors.nr());
    }

private:
	template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar & boost::serialization::base_object<learning_machine>(*this);

        std::stringstream sstr;
        dlib::serialize(normalizer_, sstr);
        dlib::serialize(learnedfunc_, sstr);
        std::string strs(sstr.str());
        reporting::report(reporting::DEBUGING) << "lm_dlib_base::save() : strs.length()=" << strs.length() << "  main_metric()=" << main_metric();
        ar << strs;
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<learning_machine>(*this);

	    std::string strs;
	    ar >> strs;
	    std::stringstream sstr;
	    sstr.str(strs);
	    dlib::deserialize(normalizer_, sstr);
        dlib::deserialize(learnedfunc_, sstr);
        reporting::report(reporting::DEBUGING) << "lm_dlib_base::load() : strs.length()=" << strs.length() << "  main_metric()=" << main_metric();
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER();

protected:
    lm_dlib_base() { } // only for serialization of derived classes
    virtual void train_algorithm(const std::vector<sample_type> &samples, const std::vector<double> &lables) = 0;

    dlib::vector_normalizer<sample_type> normalizer_;
    dlib::decision_function<kernel_type> learnedfunc_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
template<class kernel_type>
class lm_dlib_rvm : public lm_dlib_base<kernel_type>
{
    friend class boost::serialization::access;
    typedef typename kernel_type::sample_type sample_type;
public:
    lm_dlib_rvm(const std::string &pred_name, kernel_type &kern)
     : lm_dlib_base<kernel_type>(pred_name), kern_(kern) { };
    virtual ~lm_dlib_rvm() { };

    virtual void mutate(boost::function<double(double)> &real_mutator) { }
    virtual void copy_params(const learning_machine &rhs) { }

protected:
    virtual void train_algorithm(const std::vector<sample_type> &samples, const std::vector<double> &lables)
    {
        reporting::report(reporting::VERBOSE) << "train the relevance vector machine";
        dlib::rvm_regression_trainer<kernel_type> trainer;
        trainer.set_kernel(kern_);
        this->learnedfunc_ = trainer.train(samples, lables);
        reporting::report(reporting::VERBOSE) << "the resulting function has " << this->learnedfunc_.basis_vectors.nr() << " support vectors.";
    }

private:
    lm_dlib_rvm() { } // default constructor is private only for serialization

	template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar & boost::serialization::base_object<lm_dlib_base<kernel_type> >(*this);

        std::stringstream sstr;
        dlib::serialize(kern_, sstr);
        std::string strs(sstr.str());
        reporting::report(reporting::DEBUGING) << "lm_dlib_rvm::save() : strs.length()=" << strs.length();
        ar << strs;
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<lm_dlib_base<kernel_type> >(*this);

	    std::string strs;
	    ar >> strs;
	    reporting::report(reporting::DEBUGING) << "lm_dlib_rvm::load() : strs.length()=" << strs.length();
	    std::stringstream sstr;
	    sstr.str(strs);
	    dlib::deserialize(kern_, sstr);
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER();

    kernel_type kern_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
template<class kernel_type>
class lm_dlib_krls : public lm_dlib_base<kernel_type>
{
    friend class boost::serialization::access;
    typedef typename kernel_type::sample_type sample_type;
public:
    lm_dlib_krls(const std::string &pred_name, kernel_type &kern, const double fact)
     : lm_dlib_base<kernel_type>(pred_name), kern_(kern), fact_(fact) { };
    virtual ~lm_dlib_krls() { };

protected:
    virtual void train_algorithm(const std::vector<sample_type> &samples, const std::vector<double> &lables)
    {
        reporting::report(reporting::VERBOSE) << "train the kernel recursive least squares algorithm";
        dlib::krls<kernel_type> trainer(kern_, fact_);

        for(size_t i=0; i<lables.size(); ++i)
            trainer.train(samples[i], lables[i]);

        this->learnedfunc_ = trainer.get_decision_function();

        reporting::report(reporting::VERBOSE) << "the resulting function has " << this->learnedfunc_.basis_vectors.nr() << " support vectors.";
    }
private:
    lm_dlib_krls() { } // default constructor is private only for serialization

	template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar & boost::serialization::base_object<lm_dlib_base<kernel_type> >(*this);

        std::stringstream sstr;
        dlib::serialize(kern_, sstr);
        std::string strs(sstr.str());
        reporting::report(reporting::DEBUGING) << "lm_dlib_krls::save() : strs.length()=" << strs.length();
        ar << strs;
        ar << fact_;
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<lm_dlib_base<kernel_type> >(*this);

	    std::string strs;
	    ar >> strs;
	    reporting::report(reporting::DEBUGING) << "lm_dlib_krls::load() : strs.length()=" << strs.length();
	    std::stringstream sstr;
	    sstr.str(strs);
	    dlib::deserialize(kern_, sstr);
	    ar >> fact_;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER();

    kernel_type kern_;
    double fact_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // LM_SVM_DLIB_H_INCLUDED
