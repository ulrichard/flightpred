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
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
// standard library
#include <sstream>

using namespace flightpred;
using geometry::point_ll_deg;
namespace bfs = boost::filesystem;
namespace bgreg = boost::gregorian;
namespace bpt = boost::posix_time;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using boost::lexical_cast;
using std::string;
using std::vector;
using std::map;
using std::set;

// a description of the grib data fields can be found at:
// http://www.nco.ncep.noaa.gov/pmb/products/gfs/gfs.t00z.pgrb2f00.shtml
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
grib_grabber::grib_grabber(const std::string &db_conn_str, const std::string &model)
    : db_conn_str_(db_conn_str), model_(get_grib_pred_model(model))
{

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber::grab_grib(const bgreg::date &from, const bgreg::date &to, const point_ll_deg &pos)
{
    // validate that the requested location falls within the grid of the prediction model
    int lat100  = pos.lat() * 100.0;
    int lon100  = pos.lon() * 100.0;
    int grid100 = model_->getGridResolution() * 100;
    if(lat100 % grid100)
        throw std::invalid_argument("Requested position doesn't match the prediction model's grid.");
    if(lon100 % grid100)
        throw std::invalid_argument("Requested position doesn't match the prediction model's grid.");
    // check if we already have the data
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "import grib data");
    // contest
    std::stringstream sstr;
    sstr << "SELECT weather_pred_id FROM weather_pred "
         << "WHERE location = GeomFromText('POINT(" << pos.lat() << " " << pos.lon() << ")', -1) "
         << "AND pred_run='" << bgreg::to_iso_extended_string(from) << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(res.size())
    {
        std::cout << "The requested data is already present in the database. No need to download." << std::endl;
        return;
    }
    // download the grib files
    bfs::path get_gfs_pl(bfs::current_path().getParent "../flightpred/grib/get_gfs.pl");
    bfs::path gribdir("../flightpred/grib/past");
    if(!bfs::exists(get_gfs_pl))
        throw std::runtime_error("could not locate the get_gfs.pl script");
   if(!bfs::exists(gribdir))
        throw std::runtime_error("could not locate the directory to store the grib files");
    const string var_list = "all"; // or HGT:TMP ...
    const string lev_list = "surface:850_mb:750_mb:500_mb";
    for(bgreg::date day = from; day != to; day += bgreg::days(1))
    {
        std::stringstream sstr;
        sstr << "data " << bgreg::to_iso_string(day) << "00 0 72 3 " << var_list << " " << lev_list
             << " " << gribdir.external_file_string();

    }

    // add the data to the database

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
bfs::path grib_grabber::download_grib(point_ll_deg &pos)
{


}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
