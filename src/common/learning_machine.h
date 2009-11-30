#ifndef LEARNING_MACHINE_H_INCLUDED
#define LEARNING_MACHINE_H_INCLUDED

//boost
#include <boost/noncopyable.hpp>
#include <boost/range/iterator_range.hpp>
// standard library
#include <string>
#include <vector>
#include <utility>


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class learning_machine : public boost::noncopyable
{
public:
    learning_machine(const std::string &pred_name, const std::string &db_conn_str)
        : pred_name_(pred_name), db_conn_str_(db_conn_str) { }
    virtual ~learning_machine() { }

    typedef std::vector<std::vector<double> > SampleType;

    virtual void   train(const SampleType &samples, const std::vector<double> &labels) = 0;
    virtual double eval(const std::vector<double> &sample) const = 0;
    virtual void   write_to_db(const size_t conf_id) = 0;
    virtual void   read_from_db(const size_t conf_id) = 0;

protected:
    const std::string pred_name_;
    const std::string db_conn_str_;
/*
    // partition the training samples and lables into equal ranges
    template<class ForwardRange, class outContT>
    void partition_range(ForwardRange rngin, const size_t max_partition_size, std::insert_iterator<outContT> outit)
    {
        typedef typename boost::range_iterator<ForwardRange>::type iterT;
        const size_t num_part = std::max<size_t>(1, round(boost::size(rngin) / static_cast<double>(max_partition_size)));
        const size_t partition_size = boost::size(rngin) / static_cast<double>(num_part);
        iterT itbeg = boost::begin(rngin);
        for(size_t i =0; i<num_part; ++i)
        {
            iterT itend = boost::end(rngin);
            if(i <= num_part)
            {
                itend = itbeg;
                std::advance(itend, partition_size);
            }
            *outit++ = boost::make_iterator_range(itbeg, itend);
            itbeg = itend;
        }

    }
*/
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // LEARNING_MACHINE_H_INCLUDED
