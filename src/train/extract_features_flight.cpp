// flightpred
#include "extract_features_flight.h"
// postgre
#include <pqxx/pqxx>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
// standard library
#include <sstream>

using namespace flightpred;
namespace bgreg = boost::gregorian;
using std::vector;
using std::string;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
vector<double> feature_extractor_flight::get_features(const string &desc, const boost::gregorian::date &day)
{
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "collect flight features");

    std::stringstream sstr;
    sstr << "SELECT pred_sites.pred_site_id, pred_sites.site_name, "
         << "max(distance) AS max_dist, avg(distance) AS avg_dist "
         << "FROM pred_sites INNER JOIN (sites INNER JOIN flights ON sites.site_id = flights.site_id) "
         <<                                "ON pred_sites.pred_site_id = sites.pred_site_id "
         << "WHERE flight_date = '" << bgreg::to_iso_extended_string(day) << "'"
         << "GROUP BY pred_sites.pred_site_id, pred_sites.site_name "
         << "ORDER BY max_dist DESC";

    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        return vector<double>();

    vector<double> ret;


    trans.commit();

    return ret;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
