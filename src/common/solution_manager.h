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
    void evolve_population(const std::string &site_name, const size_t iterations, const double mutation_rate = 0.1);

    std::auto_ptr<solution_config> load_best_solution(const std::string &site_name, const bool onlyFullyTrained, const double maxTrainSec = 50.0);

private:
    /** @brief get an initial generation of solutions. They vary in algorithms and their parameters, but not yet by feature sets */
    std::vector<boost::shared_ptr<solution_config> > get_initial_generation(const std::string &db_conn_str);
    const bool   used_for_training(const boost::gregorian::date &day) const;
    const bool   used_for_validation(const boost::gregorian::date &day) const;
    const double test_fitness(const solution_config &sol);
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // SOLUTION_MANAGER_H_INCLUDED
