// flightpred
#include "main.h"
#include "common/features_weather.h"
#include "common/grab_grib.h"
#include "common/lm_svm_dlib.h"
#include "common/forecast.h"
// witty
#include <Wt/WEnvironment>
#include <Wt/WText>
#include <Wt/Ext/TableView>
#include <Wt/WStandardItemModel>
#include <Wt/WDate>
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
#include <boost/tuple/tuple.hpp>
#include <boost/any.hpp>
// standard library
#include <sstream>
#include <fstream>

using namespace flightpred;
namespace bgreg = boost::gregorian;
using boost::tuple;
using boost::make_tuple;
using boost::array;
using boost::any;
using std::string;
using std::vector;
using std::map;

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
        pqxx::transaction<> trans(conn, "web prediction");
        const bgreg::date today(boost::posix_time::second_clock::universal_time().date());
        const size_t num_fl_lbl = 5;
        const array<string, num_fl_lbl> svm_names = {"svm_num_flight", "svm_max_dist", "svm_avg_dist", "svm_max_dur", "svm_avg_dur"};

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

        // check if we have enough flight prediction data in the database, otherwise calculate some
        if(false)
        {

        }

        sstr.str("");
        sstr << "SELECT pred_site_id, site_name, AsText(location) as loc FROM pred_sites";
        res = trans.exec(sstr.str());
        if(!res.size())
            throw std::invalid_argument("no sites found");
        forecast fcst(db_conn_str);

        const size_t forecast_days = 3;
        Wt::WStandardItemModel *model = new Wt::WStandardItemModel(res.size(), forecast_days * 3);
        for(size_t fdays=0; fdays<forecast_days; ++fdays)
        {
            const bgreg::date day(today + bgreg::days(fdays));
            int yy = day.year();
            int mm = day.month();
            int dd = day.day();
            Wt::WDate wdate(yy, mm, dd);
            for(size_t n=0; n<3; ++n)
                model->setHeaderData(fdays * 3 + n, Wt::Horizontal, any(wdate));
        }

        for(int l=0; l<res.size(); ++l)
        {
//            size_t tmp_pred_site_id;
//            res[l]["pred_site_id"].to(tmp_pred_site_id);
//            const size_t pred_site_id = tmp_pred_site_id;
            string site_name;
            res[l]["site_name"].to(site_name);


            for(size_t j=0; j<forecast_days; ++j) // days to predict
            {
                const bgreg::date day(today + bgreg::days(j));

                map<string, double> predres = fcst.predict(site_name, day);

                // feed samples to the learned functions
                for(size_t k=0; k<num_fl_lbl; ++k)
                    model->setData(l, j * 3 + k, any(predres[svm_names[k]]));

            }
//            res[l]["loc"].to(tmpstr);
//            geometry::point_ll_deg pred_location;
//            if(!geometry::from_wkt(tmpstr, pred_location))
//                throw std::runtime_error("failed to parse the prediction site location as retured from the database : " + tmpstr);

//            sites.push_back(make_tuple(pred_site_id, site_name, pred_location));


        }

/*

        for(vector<tuple<size_t, string, geometry::point_ll_deg> >::iterator it = sites.begin(); it != sites.end(); ++it)
        {
            const size_t row_nr = std::distance(sites.begin(), it);
            std::cout << "collecting features and running SVM for " << it->get<1>() << std::endl;
            model->setHeaderData(row_nr, Wt::Vertical, any(it->get<1>()));
            sstr.str("");
            sstr << "SELECT train_sol_id, configuration, score, train_time, generation FROM trained_solutions WHERE "
                 << "pred_site_id=" << it->get<0>() << " ORDER BY score DESC";
            res = trans.exec(sstr.str());
            if(!res.size())
            {
                std::cout << "no configuration found for : " + it->get<1>() << std::endl;
                continue;
            }

            size_t sol_id;
            res[0]["train_sol_id"].to(sol_id);
            string featdesc;
            res[0]["configuration"].to(featdesc);

            // collect the current features
        //  const std::set<features_weather::feat_desc> features_weather::decode_feature_desc(featdesc);
            features_weather weather(db_conn_str);
            const std::set<features_weather::feat_desc> features = weather.get_standard_features(it->get<2>());

            // de-serialize the svm's from the database blob
            boost::ptr_vector<lm_svm_dlib> learnedfunctions;
            for(size_t j=0; j<num_fl_lbl; ++j)
            {
                learnedfunctions.push_back(new lm_svm_dlib(svm_names[j], db_conn_str));
                learnedfunctions.back().read_from_db(sol_id);
            }

            for(size_t j=0; j<forecast_days; ++j) // days to predict
            {
                const bgreg::date day(today + bgreg::days(j));
                const vector<double> valweather = weather.get_features(features, day);

                vector<double> samp;
                samp.push_back(day.year());
                samp.push_back(day.day_of_year());
                samp.push_back(day.day_of_week());
                std::copy(valweather.begin(), valweather.end(), std::back_inserter(samp));

                // feed samples to the learned functions
                array<double, num_fl_lbl> predval;
                for(size_t k=0; k<num_fl_lbl; ++k)
                {
                    predval[k] = learnedfunctions[k].eval(samp);
                    model->setData(row_nr, j * 3 + k, any(predval[k]));
                }

                sstr.str("");
                sstr << "Predicted values for " << it->get<1>() << " on " << bgreg::to_simple_string(day) << ":";
                //Wt::WText  *wtxt1 = new Wt::WText(sstr.str(), root());
                //Wt::WBreak *brk1  = new Wt::WBreak(root());

                const array<string, num_fl_lbl> labels = {"Number of flights: ", "Maximum distance: ", "Average distance: ", "Maximum duration: ", "Average duration: "};

                for(size_t k=0; k<3; ++k)
                {
                    sstr.str("");
                    sstr << labels[k] << predval[k];
                    //new Wt::WText(sstr.str(), root());
                    //new Wt::WBreak(root());
                }
                //new Wt::WBreak(root());
            } // for days to predict
        } // for sites to predict
*/

        Wt::Ext::TableView *table = new Wt::Ext::TableView(root());
        table->resize(800, 300);
        table->setModel(model);
        table->setColumnSortable(0, true);
        table->setColumnSortable(1, true);
        table->setColumnSortable(2, true);
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
