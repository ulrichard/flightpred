#ifndef TRAIN_SVM_H_INCLUDED
#define TRAIN_SVM_H_INCLUDED

//boost
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/noncopyable.hpp>
// standard library
#include <string>
#include <vector>


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class train_svm : public boost::noncopyable
{
public:
    train_svm(const std::string &db_conn_str, const size_t train_partition)
        : db_conn_str_(db_conn_str), train_partition_(train_partition) { }
    virtual ~train_svm() { }

    void train(const std::string &site_name, const boost::gregorian::date &from, const boost::gregorian::date &to);
    void train_all(const boost::gregorian::date &from, const boost::gregorian::date &to);


private:
    const std::string db_conn_str_;
    const size_t      train_partition_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // TRAIN_SVM_H_INCLUDED
