#ifndef SOLUTION_CONFIG_H_INCLUDED
#define SOLUTION_CONFIG_H_INCLUDED

// flightpred
#include <common/learning_machine.h>
#include <common/features_weather.h>
//boost
#include <boost/shared_ptr.hpp>
// std lib
#include <string>
#include <map>
#include <set>
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class solution_config
{
public:
    solution_config(const std::string &site_name, const std::string &solution_description)
        : site_name_(site_name), solution_description_(solution_description)
    {   decode();  }
    explicit solution_config(const size_t db_id);
    solution_config(const solution_config &org)
        : train_sol_id_(org.train_sol_id_), site_name_(org.site_name_), solution_description_(org.solution_description_),
          features_desc_(org.features_desc_), learning_machines_(org.learning_machines_) { }

    virtual ~solution_config() { };

    solution_config & operator=(const solution_config &org)
    {
        train_sol_id_         = org.train_sol_id_;
        site_name_            = org.site_name_;
        solution_description_ = org.solution_description_;
        features_desc_        = org.features_desc_;
        learning_machines_    = org.learning_machines_;
        return *this;
    }

    /** @brief load a solution configuration from the database. */
    static std::auto_ptr<solution_config> load_from_db(const size_t db_id)
    {    return std::auto_ptr<solution_config>(new solution_config(db_id));   }

    const size_t get_solution_id() const { return train_sol_id_; }                /// the id number in the database
    const std::string & get_site_name() const { return site_name_; }              /// flying site
    const std::string & get_description() const { return solution_description_; } /// full length with all the features
    const std::string   get_short_description() const;                            /// algorithm name, parameters and number of features
    const std::string   get_algorithm_name(const bool with_params) const;         /// algorithm name, parameters optional
    const std::set<features_weather::feat_desc> & get_weather_feature_desc() const { return features_desc_; }

    boost::shared_ptr<learning_machine> get_decision_function(const std::string &eval_name) const;

protected:
    void decode();

    size_t train_sol_id_;
    std::string site_name_;
    std::string solution_description_;
    std::set<features_weather::feat_desc> features_desc_;
    std::map<std::string, boost::shared_ptr<learning_machine> > learning_machines_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // SOLUTION_CONFIG_H_INCLUDED
