// flightpred
#include "common/train_svm.h"
#include "common/features_weather.h"
#include "common/solution_manager.h"
#include "common/flightpred_globals.h"
#include "common/reporter.h"
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/io/wkt/read_wkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/timer.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
// standard library
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace flightpred;
using namespace flightpred::reporting;
namespace bgreg = boost::gregorian;
namespace bpt   = boost::posix_time;
using boost::array;
using std::vector;
using std::set;
using std::map;
using std::pair;
using std::string;

typedef boost::geometry::model::ll::point<> point_ll_deg;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void train_svm::train(const string &site_name, const bgreg::date &from, const bgreg::date &to, const double max_val_train_time, const std::vector<std::string>& figures)
{
    report(INFO) << "train_svm::train(" << site_name << ", " << bgreg::to_iso_extended_string(from)
                 << ", " << bgreg::to_iso_extended_string(to) <<  ")";
    const std::set<boost::gregorian::date> ignored_days = solution_manager::get_ignored_days(true);
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
    point_ll_deg pred_location;
    boost::geometry::read_wkt(tmpstr, pred_location);

    // collect the labels
    report(INFO) << "collecting labels";
    sstr.str("");
    sstr << "SELECT flight_date, COUNT(*) AS " << flightpred_globals::pred_values[0]    // num_flight
         << ", MAX(distance) AS " << flightpred_globals::pred_values[1]                 // max_dist
         << ", AVG(distance) AS " << flightpred_globals::pred_values[2]                 // avg_dist
         << ", MAX(duration) AS " << flightpred_globals::pred_values[3]                 // max_dur
         << ", AVG(duration) AS " << flightpred_globals::pred_values[4]                 // avg_dur
         << " FROM flights INNER JOIN sites "
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
        for(size_t j=0; j<flightpred_globals::pred_values.size(); ++j)
        {
            double dval;
            res[i][flightpred_globals::pred_values[j]].to(dval);
            if(dval < 1.0)
                dval = 0.0;
            labelsbydate[day][j] = dval;
        }
    }
    trans1.commit();
    report(INFO) << "found " << labelsbydate.size() << " dates with flights";

    //load the configuration with the best score
    solution_manager solmgr(site_name);
    std::auto_ptr<solution_config> sol = solmgr.load_best_solution(false, max_val_train_time);

    // get the feature descriptions of the weather data
    const set<features_weather::feat_desc> &features = sol->get_weather_feature_desc();

    report(INFO) << "collecting features for " << site_name;
    learning_machine::SampleType      training_samples;
    assert(flightpred_globals::pred_values.size() == 5);
    array<vector<double>, 5> labels;
    for(bgreg::date day = from; day <= to; day += bgreg::days(1))
    {
        // we ignore some days where the grib data is incomplete
        if(ignored_days.find(day) != ignored_days.end() || ignored_days.find(day - bgreg::days(1)) != ignored_days.end() || ignored_days.find(day + bgreg::days(1)) != ignored_days.end())
            continue;

        if(labelsbydate.find(day) != labelsbydate.end())
        {
            for(size_t i=0; i<flightpred_globals::pred_values.size(); ++i)
                labels[i].push_back(labelsbydate[day][i]);
        }
        else
        {
            for(size_t i=0; i<flightpred_globals::pred_values.size(); ++i)
                labels[i].push_back(0.0);
        }

        report(INFO) << site_name << " " << bgreg::to_iso_extended_string(day)
                     << " "     << labels[0].back() << " flights "
                     << " max " << labels[1].back() << " km "
                     << " avg " << labels[2].back() << " km "
                     << " max " << labels[3].back() << " min "
                     << " avg " << labels[4].back() << " min";
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
        const string& prednam = flightpred_globals::pred_values[i];
        if(std::find(figures.begin(), figures.end(), prednam) == figures.end())
        {
            report(INFO) << "Skipping " << prednam << " at site: " << site_name;
            continue;
        }
        report(INFO) << "\nTrain the machine learning algorithm for " << prednam << " at site: " << site_name;
        report(INFO) << "This can take a long time, seriously. Starting at " << bpt::to_simple_string(bpt::second_clock::local_time()) << "\n";
        assert(training_samples.size() == labels[i].size());
        boost::timer btim;
        sol->get_decision_function(prednam)->train(training_samples, labels[i]);
        traintime += btim.elapsed();
        report(INFO) << "training took " << btim.elapsed() / 60.0 << " min" << std::endl;
        sol->get_decision_function(prednam)->write_to_db(sol->get_solution_id());
    }

    pqxx::transaction<> trans3(flightpred_db::get_conn(), "update training info");
    sstr.str("");
    sstr << "UPDATE trained_solutions SET train_time_prod=(train_time_prod+" << traintime << "), num_samples_prod=" << training_samples.size()
         << ", num_features=" << training_samples.front().size()
         << "  WHERE train_sol_id=" << sol->get_solution_id();
    res = trans3.exec(sstr.str());
    trans3.commit();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

