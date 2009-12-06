// flightpred
#include "common/solution_manager.h"
#include "common/features_weather.h"
#include "common/flightpred_globals.h"
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
#include <boost/date_time/gregorian/gregorian.hpp>
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
void solution_manager::initialize_population(const std::string &site_name)
{
    features_weather         weather;
    vector<shared_ptr<solution_config> > solutions = solution_config::get_initial_generation(site_name);
    const bgreg::date_period dates = weather.get_feature_date_period(false);
    pqxx::transaction<> trans1(flightpred_db::get_conn(), "initialize populations");

    // get the id and geographic position of the prediction site
    std::stringstream sstr;
    sstr << "SELECT pred_site_id, AsText(location) as loc FROM pred_sites WHERE site_name='" << site_name << "'";
    pqxx::result res = trans1.exec(sstr.str());
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
    trans1.exec(sstr.str());

    std::cout << "collecting labels" <<  std::endl;

    sstr.str("");
    sstr << "SELECT flight_date, MAX(distance) FROM flights INNER JOIN sites "
         << "ON flights.site_id = sites.site_id "
         << "WHERE pred_site_id = " << pred_site_id << " "
         << "GROUP BY flight_date "
         << "ORDER BY flight_date ASC";
    res = trans1.exec(sstr.str());
    map<bgreg::date, double> max_distances;
    for(size_t i=0; i<res.size(); ++i)
    {
        string datestr;
        res[i][0].to(datestr);
        bgreg::date day(bgreg::from_string(datestr));
        double max_dist;
        res[i][1].to(max_dist);
        if(max_dist < 1.0)
            max_dist = 0.0;
        max_distances[day] = max_dist;
    }
    trans1.commit();

    vector<double> training_labels;
    for(bgreg::day_iterator dit(dates.begin()); *dit <= dates.end(); ++dit)
        if(used_for_training(*dit))
            training_labels.push_back(max_distances[*dit]);

    set<set<features_weather::feat_desc> > featureconfigurations;
    BOOST_FOREACH(shared_ptr<solution_config> solution, solutions)
        featureconfigurations.insert(solution->get_weather_feature_desc());

    typedef map<set<features_weather::feat_desc>, map<bgreg::date, vector<double> > > WeatherFeatureCacheType;
    WeatherFeatureCacheType  weatherdata;
    for(set<set<features_weather::feat_desc> >::iterator fit = featureconfigurations.begin(); fit != featureconfigurations.end(); ++fit)
    {
        for(bgreg::day_iterator dit(dates.begin()); *dit <= dates.end(); ++dit)
        {
            if(used_for_training(*dit) || used_for_validation(*dit))
            {
                std::cout << "collecting features for " << bgreg::to_iso_extended_string(*dit) << std::endl;

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
    pqxx::transaction<> trans2(flightpred_db::get_conn(), "initialize populations -> train");
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
                const double & realval = max_distances[*dit];
                const double err = fabs(realval - predval);
                sum_real += realval;
                sum_err  += err;
                std::cout << bgreg::to_iso_extended_string(*dit) << " " << realval << " " << predval << " " << err << std::endl;
            }
        }
        const double perf = sum_real / sum_err;

        std::cout << "result of validation : total km = " << sum_real << " total error = " << sum_err << " perf = " << perf << std::endl;

        sstr.str("");
        sstr << "INSERT INTO trained_solutions (generation, pred_site_id, configuration, score, train_time, num_features) VALUES "
             << "(0, " << pred_site_id << ", '" << solution->get_description() << "', " << perf << ", "
             << traintime << ", " << solution->get_weather_feature_desc().size() << ")";
        trans2.exec(sstr.str());
    }

    trans2.commit();

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
std::auto_ptr<solution_config> solution_manager::load_best_solution(const std::string &site_name, const bool onlyFullyTrained, const double maxTrainSec)
{
    pqxx::transaction<> trans(flightpred_db::get_conn(), "load_best_solution");

    std::stringstream sstr;
    sstr << "SELECT pred_site_id FROM pred_sites where site_name='" << site_name << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::invalid_argument("site not found: " + site_name);

    size_t pred_site_id;
    res[0]["pred_site_id"].to(pred_site_id);

    sstr.str("");
    sstr << "SELECT train_sol_id, configuration, score, train_time, generation FROM trained_solutions WHERE ";
    if(onlyFullyTrained)
        sstr << "num_samples_prod > 300 AND ";
    if(maxTrainSec > 0.0)
        sstr << "train_time <= " << maxTrainSec << " AND ";
    sstr << "pred_site_id=" << pred_site_id << " ORDER BY score DESC, generation DESC";
    res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("no configuration found for : " + site_name);

    size_t solution_id;
    res[0]["train_sol_id"].to(solution_id);
    trans.commit();

    return solution_config::load_from_db(solution_id);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
