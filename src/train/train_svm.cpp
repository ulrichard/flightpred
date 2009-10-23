// flightpred
#include "train_svm.h"
#include "extract_features_flight.h"
#include "features_weather.h"
// postgre
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
//#include <geometry/io/wkt/aswkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
// standard library
#include <sstream>

using namespace flightpred;
namespace bgreg = boost::gregorian;
using std::vector;
using std::set;
using std::string;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void train_svm::train(const string &site_name, const bgreg::date &from, const bgreg::date &to)
{
    feature_extractor_flight flights(db_conn_str_);
    features_weather         weather(db_conn_str_);

    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "collect features");

    // get the id of the prediction site
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

    // get the feature descriptions of the weather data
//    const set<features_weather::feat_desc> features = weather.decode_feature_desc("here comes the description");
    const set<features_weather::feat_desc> features = weather.get_standard_features(pred_location);

    for(bgreg::date day = from; day <= to; day += bgreg::days(1))
    {
        vector<double> valflights = flights.get_features(pred_site_id, day);
        vector<double> valweather = weather.get_features(features, day);

    }
    trans.commit();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

