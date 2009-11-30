// flightpred
#include "common/solution_manager.h"
#include "common/features_weather.h"
#include "common/extract_features_flight.h"
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
// boost
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer.hpp>
// std lib
#include <iostream>

using namespace flightpred;
using geometry::point_ll_deg;
using boost::lexical_cast;
using boost::shared_ptr;
namespace bgreg = boost::gregorian;
using std::string;
using std::vector;
using std::map;
using std::set;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void solution_manager::initialize_populations()
{
    vector<string> sites;

    {
        pqxx::connection conn(db_conn_str_);
        pqxx::transaction<> trans(conn, "initialize populations");

        // get the id of the prediction site
        std::stringstream sstr;
        sstr << "SELECT site_name FROM pred_sites";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            throw std::invalid_argument("no sites found");
        for(size_t i=0; i<res.size(); ++i)
        {
            string tmpstr;
            res[i][0].to(tmpstr);
            sites.push_back(tmpstr);
        }
    }

    std::for_each(sites.begin(), sites.end(), boost::bind(&solution_manager::initialize_population, this, _1));
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void solution_manager::initialize_population(const std::string &site_name)
{
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "initialize populations");
    feature_extractor_flight flights(db_conn_str_);
    features_weather         weather(db_conn_str_);
    vector<shared_ptr<solution_config> > solutions = solution_config::get_initial_generation(site_name, db_conn_str_);
    const bgreg::date_period dates = weather.get_feature_date_period(false);

    // get the id and geographic position of the prediction site
    std::stringstream sstr;
    sstr << "SELECT pred_site_id, AsText(location) as loc FROM pred_sites WHERE site_name='" << site_name << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::invalid_argument("site not found : " + site_name);
    size_t tmp_pred_site_id;
    res[0]["pred_site_id"].to(tmp_pred_site_id);
    const size_t pred_site_id = tmp_pred_site_id;
    string tmpstr;
    res[0]["loc"].to(tmpstr);
    geometry::point_ll_deg pred_location;
    if(!geometry::from_wkt(tmpstr, pred_location))
        throw std::runtime_error("failed to parse the prediction site location as retured from the database : " + tmpstr);

    sstr.str("");
    sstr << "DELETE FROM trained_solutions WHERE generation=0 AND pred_site_id=" << pred_site_id;
    trans.exec(sstr.str());

    std::cout << "collecting features and labels" <<  std::endl;

    vector<double> training_labels;
    map<bgreg::date, double> validation_labels;
    for(bgreg::day_iterator dit(dates.begin()); *dit <= dates.end(); ++dit)
    {
        if(used_for_training(*dit))
        {
            const vector<double> valflights = flights.get_features(pred_site_id, *dit);
            training_labels.push_back(valflights[1]); // we're only interested in the max distance for the evolution
        }

        if(used_for_validation(*dit))
        {
            const vector<double> valflights = flights.get_features(pred_site_id, *dit);
            validation_labels[*dit] = valflights[1]; // we're only interested in the max distance for evolution
        }
    }

    set<set<features_weather::feat_desc> > featureconfigurations;
    BOOST_FOREACH(shared_ptr<solution_config> solution, solutions)
        featureconfigurations.insert(solution->get_weather_feature_desc());

    typedef map<set<features_weather::feat_desc>, map<bgreg::date, vector<double> > > WeatherFeatureCacheType;
    WeatherFeatureCacheType  weatherdata;
    for(set<set<features_weather::feat_desc> >::iterator fit = featureconfigurations.begin(); fit != featureconfigurations.end(); ++fit)
    {
        for(bgreg::day_iterator dit(dates.begin()); *dit <= dates.end(); ++dit)
        {
            if(used_for_training(*dit))
            {
                const vector<double> valweather = weather.get_features(*fit, *dit, false);
                assert(valweather.size() == fit->size());

                vector<double> &samples = weatherdata[*fit][*dit];
                // put together the values to feed to the svm
                samples.push_back(dit->year());
                samples.push_back(dit->day_of_year());
                samples.push_back(dit->day_of_week());
                std::copy(valweather.begin(), valweather.end(), std::back_inserter(samples));
            }
        }
    }

    // train
    static const string eval_name("max_dist");
    BOOST_FOREACH(shared_ptr<solution_config> solution, solutions)
    {
        std::cout << "train the support vector machine" <<  std::endl;
        learning_machine::SampleType samples;
        for(bgreg::day_iterator dit(dates.begin()); *dit <= dates.end(); ++dit)
            if(used_for_training(*dit))
                samples.push_back(weatherdata[solution->get_weather_feature_desc()][*dit]);
        boost::timer btim;
        solution->get_decision_function(eval_name)->train(samples, training_labels);
        const double traintime = btim.elapsed();
        std::cout << "training took " << btim.elapsed() << " sec" << std::endl;

        // validate
        double sum_real = 0, sum_err = 0;
        for(bgreg::day_iterator dit(dates.begin()); *dit <= dates.end(); ++dit)
        {
            if(used_for_validation(*dit))
            {
                const vector<double> &samples = weatherdata[solution->get_weather_feature_desc()][*dit];
                const double predval   = solution->get_decision_function(eval_name)->eval(samples);
                const double & realval = validation_labels[*dit];
                sum_real += realval;
                sum_err  += fabs(realval - predval);
            }
        }
        const double perf = sum_err / sum_real;

        sstr.str("");
        sstr << "INSERT INTO trained_solutions (generation, pred_site_id, configuration, score, train_time, num_features) VALUES "
             << "(0, " << pred_site_id << ", '" << solution->get_description() << "', " << perf << ", "
             << traintime << ", " << solution->get_weather_feature_desc().size() << ")";
        trans.exec(sstr.str());
    }

    trans.commit();

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const bool solution_manager::used_for_training(const bgreg::date &day) const
{
    return day.day_of_week() == 0; // sunday
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const bool solution_manager::used_for_validation(const bgreg::date &day) const
{
    return day.day_of_week() == 6; // saturday
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
