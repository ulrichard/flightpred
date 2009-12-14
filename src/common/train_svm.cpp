// flightpred
#include "common/train_svm.h"
#include "common/features_weather.h"
#include "common/solution_manager.h"
#include "common/flightpred_globals.h"
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
//#include <geometry/io/wkt/aswkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/timer.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
// standard library
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace flightpred;
namespace bgreg = boost::gregorian;
using boost::array;
using std::vector;
using std::set;
using std::map;
using std::pair;
using std::string;
using std::cout;
using std::endl;


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void train_svm::train(const string &site_name, const bgreg::date &from, const bgreg::date &to)
{
    features_weather weather(false);

    // get the id and geographic position of the prediction site
    std::stringstream sstr;
    sstr << "SELECT pred_site_id, AsText(location) as loc FROM pred_sites WHERE site_name='" << site_name << "'";
    pqxx::transaction<> trans1(flightpred_db::get_conn(), "collect features");
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

    // collect the labels
    std::cout << "collecting labels" <<  std::endl;
    sstr.str("");
    sstr << "SELECT flight_date, COUNT(*) AS num_flights, MAX(distance) AS max_dist, AVG(distance) AS avg_dist, "
         << "MAX(duration) AS max_dur, AVG(duration) AS avg_dur FROM flights INNER JOIN sites "
         << "ON flights.site_id = sites.site_id "
         << "WHERE pred_site_id = " << pred_site_id << " "
         << "GROUP BY flight_date "
         << "ORDER BY flight_date ASC";
    res = trans1.exec(sstr.str());
    map<bgreg::date, array<double, 5> > labelsbydate;
    for(size_t i=0; i<res.size(); ++i)
    {
        string datestr;
        res[i]["flight_date"].to(datestr);
        bgreg::date day(bgreg::from_string(datestr));
        double dval;
        for(size_t j=0; j<5; ++j)
        {
            res[i][1 + j].to(dval);
            if(dval < 1.0)
                dval = 0.0;
            labelsbydate[day][j] = dval;
        }
    }
    trans1.commit();
    std::cout << labelsbydate.size() << " dates with flights" << std::endl;

    //load the configuration with the best score
    solution_manager solmgr(site_name);
    std::auto_ptr<solution_config> sol = solmgr.load_best_solution(false);

    // get the feature descriptions of the weather data
    const set<features_weather::feat_desc> &features = sol->get_weather_feature_desc();
//    const set<features_weather::feat_desc> features = weather.get_standard_features(pred_location);

    cout << "collecting features for " << site_name << endl;
    learning_machine::SampleType      training_samples;
    assert(flightpred_globals::pred_values.size() == 5);
    array<vector<double>, 5> labels;
    for(bgreg::date day = from; day <= to; day += bgreg::days(1))
    {
        if(labelsbydate.find(day) != labelsbydate.end())
        {
            for(size_t i=0; i<flightpred_globals::pred_values.size(); ++i)
                labels[i].push_back(labelsbydate[day][i]);
        }
        else
            for(size_t i=0; i<flightpred_globals::pred_values.size(); ++i)
                labels[i].push_back(0.0);

        std::cout << "collecting features for " << bgreg::to_iso_extended_string(day)
                  << " "     << labels[0].back() << " flights "
                  << " max " << labels[1].back() << " km "
                  << " avg " << labels[2].back() << " km" << std::endl;
        const vector<double> valweather = weather.get_features(features, day, false);
        assert(valweather.size() == features.size());

        // put together the values to feed to the svm
        training_samples.push_back(vector<double>());
        training_samples.back().push_back(day.year());
        training_samples.back().push_back(day.day_of_year());
        training_samples.back().push_back(day.day_of_week());
        std::copy(valweather.begin(), valweather.end(), std::back_inserter(training_samples.back()));
    }

    double traintime = 0.0;
    for(size_t i=0; i<flightpred_globals::pred_values.size(); ++i)
    {
        std::cout << "train the support vector machine for " << flightpred_globals::pred_values[i] << " at site: " << site_name <<  std::endl;
        boost::timer btim;
        sol->get_decision_function(flightpred_globals::pred_values[i])->train(training_samples, labels[i]);
        traintime += btim.elapsed();
        std::cout << "training took " << btim.elapsed() / 60.0 << " min" << std::endl;
        sol->get_decision_function(flightpred_globals::pred_values[i])->write_to_db(sol->get_solution_id());
    }

    pqxx::transaction<> trans3(flightpred_db::get_conn(), "update training info");
    sstr.str("");
    sstr << "UPDATE trained_solutions SET train_time_prod=" << traintime << ", num_samples_prod=" << training_samples.size()
         << ", num_features=" << training_samples.front().size()
         << "  WHERE train_sol_id=" << sol->get_solution_id();
    res = trans3.exec(sstr.str());
    trans3.commit();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

