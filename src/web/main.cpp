// flightpred
#include "main.h"
#include "common/features_weather.h"
#include "common/grab_grib.h"
#include "common/lm_svm_dlib.h"
// witty
#include <Wt/WEnvironment>
#include <Wt/WText>
// pqxx
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
#include <geometry/io/wkt/fromwkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/array.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
// standard library
#include <sstream>
#include <fstream>

using namespace flightpred;
namespace bgreg = boost::gregorian;
using boost::array;
using std::string;
using std::vector;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// callback function is called everytime when a user enters the page. Can be used to authenticate.
Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
   // You could read information from the environment to decide
   // whether the user has permission to start a new application
    FlightpredApp *flapp = new FlightpredApp(env);

	return flapp;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// main entry point of the application
int main(int argc, char *argv[])
{
	return Wt::WRun(argc, argv, &createApplication);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// We have a FlightpredApp object for each session (user...)
FlightpredApp::FlightpredApp(const Wt::WEnvironment& env)
 : Wt::WApplication(env)
{
	setTitle("free flight prediction system");               // application title
//	useStyleSheet("flightpred.css");


    // WARNING
    // this is a messy prototype at the moment. It will improve shortly

    try
    {
        // get the feature description and the svm fro the db
        const string db_conn_str = "host=localhost dbname=flightpred user=postgres password=postgres";
        pqxx::connection conn(db_conn_str);
        pqxx::transaction<> trans(conn, "collect features");
        const bgreg::date today(boost::posix_time::second_clock::universal_time().date());

        // check if we have enough weather prediction data in the database, otherwise download some
        std::stringstream sstr;
        sstr << "SELECT * FROM weather_pred WHERE pred_time >= '"
             << bgreg::to_iso_extended_string(today + bgreg::days(1)) << " 18:00:00'";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
        {
            grib_grabber_gfs_future gribgrab(db_conn_str, 100);
            gribgrab.grab_grib(boost::posix_time::hours(24 * 4));
        }

        sstr.str("");
        sstr << "SELECT pred_site_id, site_name, AsText(location) as loc FROM pred_sites";
        res = trans.exec(sstr.str());
        if(!res.size())
            throw std::invalid_argument("no sites found");
        for(int i=0; i<res.size(); ++i)
        {
            size_t tmp_pred_site_id;
            res[i]["pred_site_id"].to(tmp_pred_site_id);
            const size_t pred_site_id = tmp_pred_site_id;
            string tmpstr;
            res[i]["site_name"].to(tmpstr);
            const string site_name = tmpstr;
            res[i]["loc"].to(tmpstr);
            geometry::point_ll_deg pred_location;
            if(!geometry::from_wkt(tmpstr, pred_location))
                throw std::runtime_error("failed to parse the prediction site location as retured from the database : " + tmpstr);

            const size_t num_fl_lbl = 5;
            const array<string, num_fl_lbl> svm_names = {"svm_num_flight", "svm_max_dist", "svm_avg_dist", "svm_max_dur", "svm_avg_dur"};
            sstr.str("");
            sstr << "SELECT train_sol_id, configuration, score, train_time, generation FROM trained_solutions WHERE "
                 << "pred_site_id=" << pred_site_id << " ORDER BY score DESC";
            res = trans.exec(sstr.str());
            if(!res.size())
                throw std::invalid_argument("no configuration found for : " + site_name);

            size_t sol_id;
            res[0]["train_sol_id"].to(sol_id);
            string featdesc;
            res[0]["configuration"].to(featdesc);

            // collect the current features
        //  const std::set<features_weather::feat_desc> features_weather::decode_feature_desc(featdesc);
            features_weather weather(db_conn_str);
            const std::set<features_weather::feat_desc> features = weather.get_standard_features(pred_location);

            // de-serialize the svm's from the database blob
            boost::ptr_vector<lm_svm_dlib> learnedfunctions;
            for(size_t i=0; i<num_fl_lbl; ++i)
            {
                learnedfunctions.push_back(new lm_svm_dlib(svm_names[i], db_conn_str));
                learnedfunctions.back().read_from_db(sol_id);
            }

            for(size_t i=0; i<3; ++i) // days to predict
            {
                const bgreg::date day(today + bgreg::days(i));
                const vector<double> valweather = weather.get_features(features, day);

                vector<double> samp;
                samp.push_back(day.year());
                samp.push_back(day.day_of_year());
                std::copy(valweather.begin(), valweather.end(), std::back_inserter(samp));

                // feed samples to the learned functions
                array<double, num_fl_lbl> predval;
                for(size_t i=0; i<num_fl_lbl; ++i)
                    predval[i] = learnedfunctions[i].eval(samp);

                sstr.str("");
                sstr << "Predicted values for " << site_name << " on " << bgreg::to_simple_string(day) << ":";
                Wt::WText  *wtxt1 = new Wt::WText(sstr.str(), root());
                Wt::WBreak *brk1  = new Wt::WBreak(root());

                const array<string, num_fl_lbl> labels = {"Number of flights: ", "Maximum distance: ", "Average distance: ", "Maximum duration: ", "Average duration: "};

                for(size_t i=0; i<3; ++i)
                {
                    sstr.str("");
                    sstr << labels[i] << predval[i];
                    new Wt::WText(sstr.str(), root());
                    new Wt::WBreak(root());
                }
                new Wt::WBreak(root());
            }
        }
    }
    catch(std::exception &ex)
    {
        std::cout << "critical error : " << ex.what() << std::endl;
         new Wt::WText(ex.what(), root());
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void FlightpredApp::finalize()
{
    // clean up
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
