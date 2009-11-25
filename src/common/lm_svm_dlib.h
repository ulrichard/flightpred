#ifndef LM_SVM_DLIB_H_INCLUDED
#define LM_SVM_DLIB_H_INCLUDED

// flightpred
#include <common/learning_machine.h>
// dlib
#include <dlib/svm.h>
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// standard library
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
template<class kernel_type>
class lm_dlib_base : public learning_machine
{
protected:
    lm_dlib_base(const std::string &pred_name, const std::string &db_conn_str, const double gamma)
      : learning_machine(pred_name, db_conn_str), gamma_(gamma) { }

public:
    typedef typename kernel_type::sample_type sample_type;

    virtual ~lm_dlib_base() { };

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
        pqxx::connection conn(db_conn_str_);
        pqxx::transaction<> trans(conn, "write svm to db");

        size_t oid_blob = 0;
        {
            pqxx::largeobject dblobj(trans);
            pqxx::olostream dbstrm(trans, dblobj);
            std::cout << "streaming normalizer and SVM to the db blob" << std::endl;
            serialize(normalizer_, dbstrm);
            dlib::serialize(learnedfunc_, dbstrm);
            oid_blob = dblobj.id();
        }

        std::stringstream sstr;
        sstr << "UPDATE trained_solutions SET " << pred_name_ << "=" << oid_blob << " WHERE train_sol_id=" << conf_id;
        trans.exec(sstr.str());
        trans.commit();

        /*
        // at the moment additionally serialize to a file
        cout << "streaming the svm to a temp file" << endl;
        std::ofstream fout("/tmp/saved_function.dat", std::ios::binary);
        serialize(learnedfunc,fout);
        fout.close();
        */
    }

    virtual void read_from_db(const size_t conf_id)
    {
        pqxx::connection conn(db_conn_str_);
        pqxx::transaction<> trans(conn, "read svm from db");

        std::stringstream sstr;
        sstr << "SELECT " << pred_name_ << " FROM trained_solutions WHERE train_sol_id=" << conf_id;
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            throw std::runtime_error("solution configuration not found");
        size_t oid_blob;
        res[0][0].to(oid_blob);

        pqxx::ilostream dbstrm(trans, oid_blob);
        assert(dbstrm.good());
        deserialize(normalizer_,  dbstrm);
        dlib::deserialize(learnedfunc_, dbstrm);

        std::cout << pred_name_ << " has " << learnedfunc_.support_vectors.nr() << " support vectors." << std::endl;
    }

protected:
    const double gamma_;
    dlib::vector_normalizer<sample_type> normalizer_;
    dlib::decision_function<kernel_type> learnedfunc_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
template<class kernel_type>
class lm_dlib_rvm : public lm_dlib_base<kernel_type>
{
    typedef typename kernel_type::sample_type sample_type;
public:
    lm_dlib_rvm(const std::string &pred_name, const std::string &db_conn_str, const double gamma)
     : lm_dlib_base<kernel_type>(pred_name, db_conn_str, gamma) { };
    virtual ~lm_dlib_rvm() { };

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

        std::cout << "normalize the samples" << std::endl;
        this->normalizer_.train(samples);
        std::transform(samples.begin(), samples.end(), samples.begin(), this->normalizer_);

        std::cout << "train therelevance vector machine" << std::endl;
        dlib::rvm_regression_trainer<kernel_type> trainer;
        trainer.set_kernel(kernel_type(this->gamma_));
        this->learnedfunc_ = trainer.train(samples, lables);
        std::cout << "the resulting function has " << this->learnedfunc_.support_vectors.nr() << " support vectors." << std::endl;
    }
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
template<class kernel_type>
class lm_dlib_krls : public lm_dlib_base<kernel_type>
{
    typedef typename kernel_type::sample_type sample_type;
public:
    lm_dlib_krls(const std::string &pred_name, const std::string &db_conn_str, const double gamma)
     : lm_dlib_base<kernel_type>(pred_name, db_conn_str, gamma) { };
    virtual ~lm_dlib_krls() { };

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
        assert(samples.size() == lables.size());

        std::cout << "normalize the samples" << std::endl;
        this->normalizer_.train(samples);
        std::transform(samples.begin(), samples.end(), samples.begin(), this->normalizer_);

        std::cout << "train the kernel recursive least squares algorithm" << std::endl;
        dlib::krls<kernel_type> trainer(kernel_type(this->gamma_), 0.001);

        for(size_t i=0; i<lables.size(); ++i)
            trainer.train(samples[i], lables[i]);

        this->learnedfunc_ = trainer.get_decision_function();

        std::cout << "the resulting function has " << this->learnedfunc_.support_vectors.nr() << " support vectors." << std::endl;
    }
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // LM_SVM_DLIB_H_INCLUDED
