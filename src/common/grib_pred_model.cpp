// flightpred
#include "grib_pred_model.h"
// ggl (boost sandbox)
//#include <boost/geometry/geometry.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <boost/foreach.hpp>
// standard library
#include <utility>
#include <algorithm>

using namespace flightpred;
using boost::geometry::point_ll_deg;
namespace bgreg = boost::gregorian;
namespace bpt = boost::posix_time;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using boost::lexical_cast;
using boost::array;
using std::string;
using std::map;
using std::set;
using std::vector;
using std::pair;
using std::make_pair;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const map<string, string> & grib_pred_model::getMeasNames() const
{
    static map<string, string> mnd;

    if(!mnd.size())
    {
        mnd["HGT"]  = "Geopotential Height [gpm]";  // 10 / 20 / 30 / 50 / 70 / 100 / 150 / 200 / 250 / 300 / 350 ... mb
        mnd["TMP"]  = "Temperature [K]";            // 10 / 20 / 30 / 50 / 70 / 100 / 150 / 200 / 250 / 300 / 350 ... mb
        mnd["UGRD"] = "U-Component of Wind [m/s]";  // 10 / 20 / 30 / 50 / 70 / 100 / 150 / 200 / 250 / 300 / 350 ... mb
        mnd["VGRD"] = "V-Component of Wind [m/s]";  // 10 / 20 / 30 / 50 / 70 / 100 / 150 / 200 / 250 / 300 / 350 ... mb
        mnd["ABSV"] = "Absolute Vorticity [1/s]";   // 10 / 20 / 30 / 50 / 70 / 100 / 150 / 200 / 250 / 300 / 350 ... mb
        mnd["O3MR"] = "Ozone Mixing Ratio [kg/kg]"; // 10 / 20 / 30 / 50 / 70 / 100 mb
        mnd["RH"]   = "Relative Humidity [%]";      //                          100 / 150 / 200 / 250 / 300 / 350 ... mb
        mnd["VVEL"] = "Vertical Velocity (Pressure) [Pa/s]"; //                 100 / 150 / 200 / 250 / 300 / 350 ... mb
        mnd["CLWMR"] = "Cloud Mixing Ratio [kg/kg]"; //                         100 / 150 / 200 / 250 / 300 / 350 ... mb
//      mnd[""] = "";
    }

    return mnd;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const set<size_t> & grib_pred_model_gfs::getPressureLevels() const
{
    static set<size_t> plev;

    if(!plev.size())
    {
        plev.insert(10);
        plev.insert(20);
        plev.insert(30);
        plev.insert(50);
        plev.insert(70);
        plev.insert(100);
        plev.insert(150);
        plev.insert(200);
        plev.insert(250);
        plev.insert(300);
        plev.insert(350);
        plev.insert(400);
        plev.insert(450);
        plev.insert(500);
        plev.insert(550);
        plev.insert(600);
        plev.insert(650);
        plev.insert(700);
        plev.insert(750);
        plev.insert(800);
        plev.insert(850);
        plev.insert(900);
        plev.insert(925);
        plev.insert(950);
        plev.insert(975);
        plev.insert(1000);
        plev.insert(0);      // surface  -> PRES / HGT / TMP
    }

    return plev;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const set<time_duration> & grib_pred_model_gfs::getPredRunTimes() const
{
    static set<time_duration> runtimes;

    if(!runtimes.size())
    {
        runtimes.insert(time_duration( 0, 0, 0, 0));
        runtimes.insert(time_duration( 6, 0, 0, 0));
        runtimes.insert(time_duration(12, 0, 0, 0));
        runtimes.insert(time_duration(18, 0, 0, 0));
    }

    return runtimes;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const set<time_duration> & grib_pred_model_gfs::getFutureTimes() const
{
    static set<time_duration> futimes;

    if(!futimes.size())
    {
        for(int i=0; i<72; i += 3)
            futimes.insert(time_duration(i, 0, 0, 0));
    }

    return futimes;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
vector<pair<size_t, string> >  grib_pred_model_gfs::getStandardProperties() const
{
    vector<pair<size_t, string> > vprop;

    vprop.push_back(make_pair(0,   "PRES")); // surface pressure
    vprop.push_back(make_pair(0,   "TMP"));  // surface temperature
    array<size_t, 3> levels = {850, 700, 500};
    BOOST_FOREACH(size_t lvl, levels)
    {
        vprop.push_back(make_pair(lvl, "HGT"));
        vprop.push_back(make_pair(lvl, "TMP"));
        vprop.push_back(make_pair(lvl, "UGRD"));
        vprop.push_back(make_pair(lvl, "VGRD"));
        vprop.push_back(make_pair(lvl, "ABSV"));
        vprop.push_back(make_pair(lvl, "RH"));
        vprop.push_back(make_pair(lvl, "VVEL"));
        vprop.push_back(make_pair(lvl, "CLWMR"));
    }

    return vprop;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
grib_measure::grib_measure(const grib_pred_model *pred_model, const point_ll_deg &pos,
      size_t lvl, const std::string &prop, double val, const ptime &pred_start, const time_duration &pred_future)
    : pred_model_(pred_model), pos_(pos), level_(lvl), prop_(prop), pred_start_(pred_start), pred_future_(pred_future), val_(val)
{
    const map<string, string> &menam = pred_model_->getMeasNames();
    if(menam.find(prop) == menam.end())
        throw std::invalid_argument("Invalid property : " + prop);
    const set<size_t> &lev = pred_model_->getPressureLevels();
    if(lev.find(lvl) == lev.end())
        throw std::invalid_argument("Invalid pressure level : " + lexical_cast<string>(lvl));
    const set<time_duration> &runtimes = pred_model_->getPredRunTimes();
    time_duration runtime = pred_start_.time_of_day();
    if(runtimes.find(runtime) == runtimes.end())
        throw std::invalid_argument("Invalid prediction run time : " + bpt::to_simple_string(pred_start_));
    const set<time_duration> &futimes = pred_model_->getFutureTimes();
    if(futimes.find(pred_future_) == futimes.end())
        throw std::invalid_argument("Invalid prediction future time : " + bpt::to_simple_string(pred_future_));
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
grib_pred_model * flightpred::get_grib_pred_model(const std::string &name)
{
    static grib_pred_model_gfs *gfs = new grib_pred_model_gfs();

    if(name == "GFS")
        return gfs;

    throw std::invalid_argument("weather prediction model not supported.");
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

