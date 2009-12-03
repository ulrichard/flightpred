#ifndef SOLUTION_MANAGER_H_INCLUDED
#define SOLUTION_MANAGER_H_INCLUDED

// flightpred
#include <common/solution_config.h>
//boost
#include <boost/noncopyable.hpp>
//#include <boost/shared_ptr.hpp>
// std lib
#include <string>
#include <map>
#include <set>
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class solution_manager : public boost::noncopyable
{
public:
    solution_manager() { }
    virtual ~solution_manager() { };

    void initialize_population(const std::string &site_name);
    void initialize_populations();


private:
    const bool used_for_training(const boost::gregorian::date &day) const;
    const bool used_for_validation(const boost::gregorian::date &day) const;

    const std::vector<boost::gregorian::date> training_days;
    const std::vector<boost::gregorian::date> validation_days;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // SOLUTION_MANAGER_H_INCLUDED
