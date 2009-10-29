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
vector<double> feature_extractor_flight::get_features(size_t pred_site_id, const boost::gregorian::date &day)
{
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "collect flight features");

    std::stringstream sstr;
    sstr << "SELECT max(distance) AS max_dist, avg(distance) AS avg_dist, "
         << "max(duration) as max_dur, avg(duration) as avg_dur "
         << "FROM sites INNER JOIN flights ON sites.site_id = flights.site_id "
         << "WHERE flight_date = '" << bgreg::to_iso_extended_string(day) << "' "
         << "AND pred_site_id=" << pred_site_id;
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        return vector<double>();

    vector<double> ret;

    double val;
    res[0]["max_dist"].to(val);
    ret.push_back(val);
    res[0]["avg_dist"].to(val);
    ret.push_back(val);
    res[0]["max_dur"].to(val);
    ret.push_back(val);
    res[0]["avg_dur"].to(val);
    ret.push_back(val);

    return ret;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A