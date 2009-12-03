#ifndef SOLUTION_CONFIG_H_INCLUDED
#define SOLUTION_CONFIG_H_INCLUDED

// flightpred
#include <common/learning_machine.h>
#include <common/features_weather.h>
//boost
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
// std lib
#include <string>
#include <map>
#include <set>
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class solution_config : public boost::noncopyable
{
public:
    virtual ~solution_config() { };

    static std::vector<boost::shared_ptr<solution_config> > get_initial_generation(const std::string &db_conn_str);

    const std::string & get_description() const { return solution_description_; }
    const std::set<features_weather::feat_desc> & get_weather_feature_desc() const { return features_desc_; }

    boost::shared_ptr<learning_machine> get_decision_function(const std::string &eval_name);

protected:
    solution_config(const std::string &site_name, const std::string &solution_description)
        : site_name_(site_name), solution_description_(solution_description)
    {   decode();  }

    void decode();


    const std::string site_name_;
    std::string solution_description_;
    std::set<features_weather::feat_desc> features_desc_;
    std::map<std::string, boost::shared_ptr<learning_machine> > learning_machines_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // SOLUTION_CONFIG_H_INCLUDED
