// flightpred
#include "main.h"
#include "common/features_weather.h"
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
// standard library
#include <sstream>

using namespace flightpred;
namespace bgreg = boost::gregorian;
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


    sstr.str("");
    sstr << "SELECT configuration, svm_ser, score, train_time, generation FROM trained_solutions WHERE "
         << "pred_site_id=" << pred_site_id << " ORDER BY score DESC";
    res = trans.exec(sstr.str());
    if(!res.size())
        throw std::invalid_argument("no configuration found for : " + site_name);

    typedef dlib::matrix<double, 0, 1> sample_type;
    typedef dlib::radial_basis_kernel<sample_type> kernel_type;
    dlib::decision_function<kernel_type> learnedfunc;
    int loid;
    res[0]["svm_ser"].to(loid);
    string featdesc;
    res[0]["configuration"].to(featdesc);

    // de-serialize the svm from the database blob
    pqxx::ilostream dbstrm(trans, loid);
    dlib::deserialize(learnedfunc, dbstrm);

//  const std::set<features_weather::feat_desc> features_weather::decode_feature_desc(featdesc);
    features_weather weather(db_conn_str);
    const std::set<features_weather::feat_desc> features = weather.get_standard_features(pred_location);

    const bgreg::date day(2009, 9, 1);
    const vector<double> valweather = weather.get_features(features, day);

    sample_type samp;
    samp(0) = day.year();
    samp(1) = day.day_of_year();
    samp.set_size(valweather.size() + 2);
    for(size_t i=0; i<valweather.size(); ++i)
        samp(i + 2) = valweather[i];

    double pred_dist = learnedfunc(samp);

    sstr.str("");
    sstr << "Predicted distance for " << site_name << " on " << bgreg::to_simple_string(day) << " is " << pred_dist << " km.";
    Wt::WText *wtxt = new Wt::WText(sstr.str(), root());



}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void FlightpredApp::finalize()
{
    // clean up
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
