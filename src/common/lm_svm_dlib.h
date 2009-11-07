#ifndef LM_SVM_DLIB_H_INCLUDED
#define LM_SVM_DLIB_H_INCLUDED

// flightpred
#include <common/learning_machine.h>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class lm_svm_dlib_impl;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class lm_svm_dlib : public learning_machine
{
public:
    lm_svm_dlib(const std::string &pred_name, const std::string &db_conn_str, const size_t train_partition);
    virtual ~lm_svm_dlib();

    virtual void   train(const SampleType &samples, const std::vector<double> &labels);
    virtual double eval(const std::vector<double> &sample) const;
    virtual void   write_to_db(const size_t conf_id);
    virtual void   read_from_db(const size_t conf_id);
private:
    lm_svm_dlib_impl *pimpl_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // LM_SVM_DLIB_H_INCLUDED
