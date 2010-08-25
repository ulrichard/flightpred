// flightpred
#include "forecast.h"
#include "common/features_weather.h"
#include "common/solution_manager.h"
#include "common/flightpred_globals.h"
#include "common/reporter.h"
// postgre
#include <pqxx/pqxx>
//#include <pqxx/largeobject>
// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/io/wkt/read_wkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/foreach.hpp>
// standard library
#include <sstream>
#include <fstream>
#include <algorithm>
#include <utility>
#include <limits>

using namespace flightpred;
using namespace flightpred::reporting;
namespace bgreg = boost::gregorian;
using boost::array;
using std::vector;
using std::string;
using std::map;
using std::set;
using std::pair;
using std::numeric_limits;


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void forecast::prediction_run(const string &site_name, const size_t pred_days)
{
    report(INFO) << "Running forecast for : " << site_name;
    try
    {
        pqxx::transaction<> trans1(flightpred_db::get_conn(), "flight prediction");
        const bgreg::date today(boost::posix_time::second_clock::universal_time().date());

        std::stringstream sstr;
        sstr << "SELECT pred_site_id FROM pred_sites WHERE site_name='" << site_name << "'";
        pqxx::result res = trans1.exec(sstr.str());
        if(!res.size())
            throw std::invalid_argument("site not found: " + site_name);
        size_t pred_site_id;
        res[0]["pred_site_id"].to(pred_site_id);
        trans1.commit();

        features_weather weather(false);
        //load the configuration with the best score
        solution_manager solmgr(site_name);
        std::auto_ptr<solution_config> sol = solmgr.load_best_solution(true);

        // get the feature descriptions of the weather data
        const set<features_weather::feat_desc> &features = sol->get_weather_feature_desc();

        for(size_t j=0; j<pred_days; ++j)
        {
            const bgreg::date day(today + bgreg::days(j));

            const vector<double> valweather = weather.get_features(features, day, true);
            assert(valweather.size() == features.size());

            // put together the values to feed to the svm
            vector<double> features;
            features.push_back(day.year());
            features.push_back(day.day_of_year());
            features.push_back(day.day_of_week());
            std::copy(valweather.begin(), valweather.end(), std::back_inserter(features));

            // let the machine make it's (educated) guesses
            map<string, double> predval;
            for(size_t k=0; k<flightpred_globals::pred_values.size(); ++k)
            {
                double val = sol->get_decision_function(flightpred_globals::pred_values[k])->eval(features);
                if(isnan(val) || val < 0.0)
                   val = 0.0;
                predval[flightpred_globals::pred_values[k]] = val;
            }

            sstr.str("");
            sstr << "INSERT INTO flight_pred (pred_site_id, train_sol_id,";
            std::copy(flightpred_globals::pred_values.begin(), flightpred_globals::pred_values.end(), std::ostream_iterator<string>(sstr, ", "));
            sstr << "calculated, pred_day) VALUES (" << pred_site_id << ", " << sol->get_solution_id() << ", ";

            BOOST_FOREACH(string predname, flightpred_globals::pred_values)
            {
                double val = predval[predname];
                sstr << val << ", ";
                std::cout << site_name << " " << bgreg::to_iso_extended_string(day) << " "
                          << predname << " " << val << std::endl;
            }

            sstr << "Now(), '" <<  bgreg::to_iso_extended_string(day) << "')";
            pqxx::transaction<> transl(flightpred_db::get_conn(), "flight prediction");
            res = transl.exec(sstr.str());
            transl.commit();
        }
    }
    catch(std::exception &ex)
    {
        report(ERROR) << "critical error : " << ex.what();
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

