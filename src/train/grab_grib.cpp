// flightpred
#include "grab_grib.h"
#include "geo_parser.h"
// postgre
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/geometry.hpp>
#include <geometry/io/wkt/streamwkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace flightpred;
using geometry::point_ll_deg;
namespace bfs = boost::filesystem;
namespace bgreg = boost::gregorian;

// a description of the grib data fields can be found at:
// http://www.nco.ncep.noaa.gov/pmb/products/gfs/gfs.t00z.pgrb2f00.shtml
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
grib_grabber::grib_grabber(const std::string &db_conn_str)
    : db_conn_str_(db_conn_str), grib_fields_("")
{

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber::grab_grib(const boost::gregorian::date &from, const boost::gregorian::date &to, const point_ll_deg &pos)
{

    // check if we already have the data
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "import grib data");
    // contest
    std::stringstream sstr;
    sstr << "SELECT weather_pred_id FROM weather_pred "
         << "WHERE location=point(" << pos.lat() << "'" << pos.lon() << ") "
         << "AND pred_run='" << bgreg::to_iso_extended_string(from) << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(res.size())
        return;

    // download the grib files

    // add the data to the database

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
bfs::path grib_grabber::download_grib(point_ll_deg &pos)
{


}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
