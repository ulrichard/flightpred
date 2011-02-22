#ifndef LEARNING_MACHINE_H_INCLUDED
#define LEARNING_MACHINE_H_INCLUDED

//boost
#include <boost/noncopyable.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/function.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/split_member.hpp>
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
    friend class boost::serialization::access;
public:
    virtual ~learning_machine() { }

    typedef std::vector<std::vector<double> > SampleType;

    virtual void        train(const SampleType &samples, const std::vector<double> &labels) = 0;
    virtual double      eval(const std::vector<double> &sample) const = 0;
    virtual void        write_to_db(const size_t conf_id) = 0;
    virtual void        read_from_db(const size_t conf_id) = 0;
    virtual int         main_metric() const = 0; // can be number of support vectors, number of neurons or whatever
    virtual std::string description() const = 0;

private:
    template<class Archive> void serialize(Archive &ar, const unsigned int version)
	{
		ar & const_cast<std::string&>(pred_name_);
	}

protected:
    learning_machine() { } // only for serialization of derived classes
    explicit learning_machine(const std::string &pred_name)
        : pred_name_(pred_name) { }

    const std::string pred_name_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // LEARNING_MACHINE_H_INCLUDED
