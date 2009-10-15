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
#include <boost/tokenizer.hpp>
// standard library
#include <sstream>
#include <iomanip>


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
    const string baseurl = "http://nomad3.ncep.noaa.gov/pub/reanalysis-2/6hr/pgb/pgb.";

    set<string> sel_levels;
    sel_levels.insert("surface");
    sel_levels.insert("850 mb");
    sel_levels.insert("750 mb");
    sel_levels.insert("500 mb");


    for(bgreg::date mon(from.year(), from.month(), 1); mon <= to; mon += bgreg::months(1))
    {
        // first, get the inventory
        std::stringstream sstr;
        sstr << "curl " << baseurl << std::setfill('0') << std::setw(4) << static_cast<int>(mon.year())
             << std::setw(2) << static_cast<int>(mon.month()) << ".inv";

        // ugly C code
        FILE *fpipe = static_cast<FILE*>(popen(sstr.str().c_str(),"r"));
        if(!fpipe)
            throw std::runtime_error("failed to read pipe from downloading grib inventory file.");

        // the line format looks something like :
        // 14559:182283710:d=2009093006:VGRD:10 mb:anl:NAve=0:range=182283710-182298255
        char line[1024 * 5];
        while(fgets(line, sizeof(line), fpipe))
        {
            const string strline(line);
            boost::char_separator<char> sep(":");
            boost::tokenizer<boost::char_separator<char> > tokens(strline, sep);
            vector<string> vtokens;
            std::copy(tokens.begin(), tokens.end(), back_inserter(vtokens));
            if(vtokens.size() < 8)
                continue;
            if(sel_levels.find(vtokens[4]) == sel_levels.end())
                continue;
            const string rng = vtokens[7].substr(6, vtokens[7].length());
            size_t pos = rng.find('-');
            if(pos = string::npos)
                continue;
            size_t rng_start = lexical_cast<size_t>(rng.substr(0, pos-1));
            size_t rng_end   = lexical_cast<size_t>(rng.substr(pos + 1, rng.length()));

            std::cout << rng_start << " " << rng_end << std::endl;

        }
        pclose(fpipe);

    }


/*
    // present : http://www.ftp.ncep.noaa.gov/data/nccf/com/gfs/prod
    const bfs::path project_dir(bfs::current_path().parent_path().parent_path());
    bfs::path get_gfs_pl(project_dir / "grib/get_gfs.pl");
    bfs::path gribdir(project_dir / "grib/past");
    if(!bfs::exists(get_gfs_pl))
        throw std::runtime_error("could not locate the get_gfs.pl script");
   if(!bfs::exists(gribdir))
        throw std::runtime_error("could not locate the directory to store the grib files");
    const string var_list = "all"; // or HGT:TMP ...
    const string lev_list = "surface:850_mb:750_mb:500_mb";
    const set<bpt::time_duration> &runtimes = model_->getPredRunTimes();
    for(bgreg::date day = from; day != to; day += bgreg::days(1))
        for(set<bpt::time_duration>::const_iterator rtit = runtimes.begin(); rtit != runtimes.end(); ++rtit)
        {
            std::stringstream sstr;
            sstr << get_gfs_pl.external_file_string() << " data " << bgreg::to_iso_string(day)
                 << std::setfill('0') << std::setw(2) << rtit->hours() << " 0 72 3 "
                 << var_list << " " << lev_list << " " << gribdir.external_file_string();
            const string syscmd(sstr.str());
            system(syscmd.c_str());
        }
*/
    // add the data to the database

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
bfs::path grib_grabber::download_grib(point_ll_deg &pos)
{


}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
