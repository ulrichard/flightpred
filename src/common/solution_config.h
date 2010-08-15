#ifndef SOLUTION_CONFIG_H_INCLUDED
#define SOLUTION_CONFIG_H_INCLUDED

// flightpred
#include <common/learning_machine.h>
#include <common/features_weather.h>
//boost
#include <boost/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
//#include <boost/serialization/export.hpp>
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
    friend class boost::serialization::access;
public:
    solution_config(const std::string &site_name, const std::string &solution_description, size_t generation)
        : train_sol_id_(0), generation_(generation), site_name_(site_name), solution_description_(solution_description)
    {   decode();  }
    explicit solution_config(const size_t db_id);
    solution_config(const solution_config &org)
        : train_sol_id_(org.train_sol_id_), generation_(org.generation_), site_name_(org.site_name_),
          solution_description_(org.solution_description_), features_desc_(org.features_desc_), learning_machines_(org.learning_machines_)
    { }

    virtual ~solution_config() { };

    solution_config & operator=(const solution_config &org)
    {
        train_sol_id_         = org.train_sol_id_;
        generation_           = org.generation_;
        site_name_            = org.site_name_;
        solution_description_ = org.solution_description_;
        features_desc_        = org.features_desc_;
        learning_machines_    = org.learning_machines_;
        return *this;
    }

    size_t get_solution_id() const { return train_sol_id_; }                /// the id number in the database
    size_t get_generation()  const { return generation_; }                  /// the generation in the evolution
    const std::string& get_site_name() const { return site_name_; }              /// flying site
    const std::string& get_description() const { return solution_description_; } /// full length with all the features
    const std::string  get_short_description() const;                            /// algorithm name, parameters and number of features
    const std::string  get_algorithm_name(const bool with_params) const;         /// algorithm name, parameters optional
    const std::set<features_weather::feat_desc> & get_weather_feature_desc() const { return features_desc_; }

    boost::shared_ptr<learning_machine> get_decision_function(const std::string &eval_name) const;

private:
    void write_to_db();
    solution_config() { }  // default constructable only for serialization
    template<class Archive> void serialize(Archive &ar, const unsigned int version)
	{
		ar & train_sol_id_;
		ar & generation_;
		ar & site_name_;
		ar & solution_description_;
		ar & learning_machines_;

        if(Archive::is_loading::value)
		{
		     decode();
		     // insert or overwrite
		     write_to_db();
		}

	}

protected:
    void decode();

    size_t train_sol_id_;
    size_t generation_;
    std::string site_name_;
    std::string solution_description_;
    std::set<features_weather::feat_desc> features_desc_;
    std::map<std::string, boost::shared_ptr<learning_machine> > learning_machines_;
    static const std::string rgxreal_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/*
// for serialization, declare all solution configuration classes that are supported
BOOST_CLASS_EXPORT((lm_dlib_rvm<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > >));
BOOST_CLASS_EXPORT((lm_dlib_rvm<dlib::sigmoid_kernel<dlib::matrix<double, 0, 1> > >));
BOOST_CLASS_EXPORT((lm_dlib_rvm<dlib::polynomial_kernel<dlib::matrix<double, 0, 1> > >));
BOOST_CLASS_EXPORT((lm_dlib_krls<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > >));
*/
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // SOLUTION_CONFIG_H_INCLUDED
