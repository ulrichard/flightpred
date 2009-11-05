// flightpred
#include "main.h"
#include "common/features_weather.h"
#include "common/grab_grib.h"
// witty
#include <Wt/WEnvironment>
#include <Wt/WText>
// pqxx
#include <pqxx/pqxx>
// dlib
#include <dlib/svm.h>
// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
#include <geometry/io/wkt/fromwkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/array.hpp>
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

        const string site_name = "Fiesch";
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

        const size_t num_fl_lbl = 5;
        const array<string, num_fl_lbl> svm_names = {"svm_num_flight", "svm_max_dist", "svm_avg_dist", "svm_max_dur", "svm_avg_dur"};
        sstr.str("");
        sstr << "SELECT configuration, ";
        std::copy(svm_names.begin(), svm_names.end(), std::ostream_iterator<string>(sstr, ", "));
        sstr << "normalizer, score, train_time, generation FROM trained_solutions WHERE "
             << "pred_site_id=" << pred_site_id << " ORDER BY score DESC";
        res = trans.exec(sstr.str());
        if(!res.size())
            throw std::invalid_argument("no configuration found for : " + site_name);

        string featdesc;
        res[0]["configuration"].to(featdesc);
        array<size_t, num_fl_lbl> blobids;
        for(size_t i=0; i<num_fl_lbl; ++i)
            res[0][svm_names[i]].to(blobids[i]);
        size_t oid_normalizer;
        res[0]["normalizer"].to(oid_normalizer);

        const bgreg::date today(boost::posix_time::second_clock::universal_time().date());

        // check if we have enough weather prediction data in the database
        sstr.str("");
        sstr << "SELECT * FROM weather_pred WHERE pred_time >= '"
             << bgreg::to_iso_extended_string(today + bgreg::days(2)) << " 18:00:00'";
        res = trans.exec(sstr.str());
        if(!res.size())
        {
            grib_grabber_gfs_future gribgrab(db_conn_str, 100);
            gribgrab.grab_grib();
        }

        typedef dlib::matrix<double, 0, 1> sample_type;
        typedef dlib::radial_basis_kernel<sample_type> kernel_type;
        dlib::decision_function<kernel_type> learnedfunc;

        // collect the current features
    //  const std::set<features_weather::feat_desc> features_weather::decode_feature_desc(featdesc);
        features_weather weather(db_conn_str);
        const std::set<features_weather::feat_desc> features = weather.get_standard_features(pred_location);

        for(size_t i=0; i<3; ++i)
        {
            const bgreg::date day(today + bgreg::days(i));
            const vector<double> valweather = weather.get_features(features, day);

            sample_type samp;
            samp.set_size(valweather.size() + 2);
            samp(0) = day.year();
            samp(1) = day.day_of_year();
            for(size_t i=0; i<valweather.size(); ++i)
                samp(i + 2) = valweather[i];

            // normalize the samples
            dlib::vector_normalizer<sample_type> normalizer;
            pqxx::ilostream dbstrm(trans, oid_normalizer);
            assert(dbstrm.good());
            deserialize(normalizer, dbstrm);
            samp = normalizer(samp);

            // fedd samples to the learned functions
            array<double, num_fl_lbl> predval;
            for(size_t i=0; i<num_fl_lbl; ++i)
            {
                // de-serialize the svm's from the database blob
                pqxx::ilostream dbstrm(trans, blobids[i]);
                assert(dbstrm.good());
                dbstrm.seekg(0, std::ios::end);
                const size_t strmsize = dbstrm.tellg();
                dbstrm.seekg(0, std::ios::beg);
                std::cout << "size of the db stream " << strmsize << std::endl;
                dlib::deserialize(learnedfunc, dbstrm);

                predval[i] = learnedfunc(samp);
            }

            sstr.str("");
            sstr << "Predicted values for " << site_name << " on " << bgreg::to_simple_string(day) << ":";
            Wt::WText  *wtxt1 = new Wt::WText(sstr.str(), root());
            Wt::WBreak *brk1  = new Wt::WBreak(root());

            const array<string, num_fl_lbl> labels = {"Number of flights: ", "Maximum distance: ", "Average distance: ", "Maximum duration: ", "Average duration: "};

    //        root().clear();
            for(size_t i=0; i<3; ++i)
            {
                sstr.str("");
                sstr << labels[i] << predval[i];
                new Wt::WText(sstr.str(), root());
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
