// flightpred
#include "lm_svm_dlib.h"
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// dlib
#include <dlib/svm.h>
// boost
#include <boost/array.hpp>
#include <boost/bind.hpp>
// standard library
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <numeric>

using namespace flightpred;
using boost::array;
using std::vector;
using std::string;
using std::cout;
using std::endl;


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
class lm_svm_dlib_impl : public learning_machine
{
public:
    typedef dlib::matrix<double, 0, 1> sample_type;
    typedef dlib::radial_basis_kernel<sample_type> kernel_type;

    lm_svm_dlib_impl(const string &pred_name, const std::string &db_conn_str, const size_t train_partition)
        : learning_machine(pred_name, db_conn_str), train_partition_(train_partition) { }
    virtual ~lm_svm_dlib_impl() {}

    // inherited from learning_machine
    virtual void   train(const SampleType &samples, const vector<double> &labels);
    virtual double eval(const std::vector<double> &sample) const;
    virtual void   write_to_db(const size_t conf_id);
    virtual void   read_from_db(const size_t conf_id);

    const size_t train_partition_;
    dlib::vector_normalizer<sample_type> normalizer_;
    dlib::decision_function<kernel_type> learnedfunc_;
};
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void lm_svm_dlib_impl::train(const learning_machine::SampleType &samplesin, const vector<double> &labels)
{
    assert(samplesin.size() == labels.size());

    // partition the data
    typedef learning_machine::SampleType::const_iterator iterTsamp;
    vector<boost::iterator_range<iterTsamp> > ranges_samp;
    partition_range(samplesin, train_partition_, std::inserter(ranges_samp, ranges_samp.end()));
    typedef vector<double>::const_iterator iterTlbl;
    vector<boost::iterator_range<iterTlbl> > ranges_lbl;
    partition_range(labels, train_partition_, std::inserter(ranges_lbl, ranges_lbl.end()));
    assert(ranges_samp.size() == ranges_lbl.size());
    cout << "splitted the " << samplesin.size() << " samples into " << ranges_samp.size() << " partitions." << endl;



    // format transformation
    vector<sample_type> samples;
    for(SampleType::const_iterator it = samplesin.begin(); it != samplesin.end(); ++it)
    {
        sample_type samp;
        samp.set_size(it->size());
        for(size_t i=0; i<it->size(); ++i)
            samp(i) = (*it)[i];
        samples.push_back(samp);
    }

    // normalize the samples
    cout << "normalize the samples" << endl;
    normalizer_.train(samples);
    std::transform(samples.begin(), samples.end(), samples.begin(), normalizer_);

    cout << "train the support vector machine" << endl;
    dlib::rvm_regression_trainer<kernel_type> trainer;
    trainer.set_kernel(kernel_type(0.05));
    learnedfunc_ = dlib::reduced2(trainer, train_partition_ / 3).train(samples, labels);
    cout << "the resulting SVM has " << learnedfunc_.support_vectors.nr() << " support vectors." << endl;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void lm_svm_dlib_impl::write_to_db(const size_t conf_id)
{
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "write svm to db");

    size_t oid_blob = 0;
    {
        pqxx::largeobject dblobj(trans);
        pqxx::olostream dbstrm(trans, dblobj);
        cout << "streaming normalizer and SVM to the db blob" << endl;
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
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void lm_svm_dlib_impl::read_from_db(const size_t conf_id)
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

    cout << pred_name_ << " has " << learnedfunc_.support_vectors.nr() << " support vectors." << endl;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
double lm_svm_dlib_impl::eval(const vector<double> &sample) const
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
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
lm_svm_dlib::lm_svm_dlib(const string &pred_name, const string &db_conn_str, const size_t train_partition)
    : learning_machine(pred_name, db_conn_str)
{
    pimpl_ = new lm_svm_dlib_impl(pred_name, db_conn_str, train_partition);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
lm_svm_dlib::~lm_svm_dlib()
{
    delete pimpl_;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void lm_svm_dlib::train(const SampleType &samples, const vector<double> &labels)
{
    pimpl_->train(samples, labels);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
double lm_svm_dlib::eval(const vector<double> &sample) const
{
    return pimpl_->eval(sample);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void lm_svm_dlib::write_to_db(const size_t conf_id)
{
    pimpl_->write_to_db(conf_id);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void lm_svm_dlib::read_from_db(const size_t conf_id)
{
    pimpl_->read_from_db(conf_id);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
