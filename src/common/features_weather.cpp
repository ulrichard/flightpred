// flightpred
#include "features_weather.h"
#include "grib_pred_model.h"
//#include "area_mgr.h"
// postgre
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
#include <geometry/io/wkt/aswkt.hpp>
#include <geometry/util/graticule.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
// standard library
#include <vector>
#include <map>
#include <sstream>

using namespace flightpred;
using geometry::point_ll_deg;
namespace bgreg = boost::gregorian;
namespace bpt   = boost::posix_time;
using boost::array;
using std::vector;
using std::set;
using std::map;
using std::string;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// the following function is only here as long as it's not part of the ggl
namespace geometry
{
// Formula to calculate the point at a distance/angle from another point
// This might be a GGL-function in the future.
template <typename P1, typename P2>
inline void point_at_distance(P1 const& p1, double distance, double tc, double radius, P2& p2)
{
    double earth_perimeter = radius * geometry::math::two_pi;
    double d = (distance / earth_perimeter) * geometry::math::two_pi;
    double const& lat1 = geometry::get_as_radian<1>(p1);
    double const& lon1 = geometry::get_as_radian<0>(p1);

    // http://williams.best.vwh.net/avform.htm#LL
    double lat = asin(sin(lat1)*cos(d)+cos(lat1)*sin(d)*cos(tc));
    double dlon = atan2(sin(tc)*sin(d)*cos(lat1),cos(d)-sin(lat1)*sin(lat));
    double lon = lon1 - dlon;

    geometry::set_from_radian<1>(p2, lat);
    geometry::set_from_radian<0>(p2, lon);
}
} // namespace geometry
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
set<features_weather::feat_desc> features_weather::decode_feature_desc(const std::string &desc)
{
    // todo : implement
    set<feat_desc> features;
    return features;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void features_weather::generate_features(features_weather::feat_desc feat, const vector<point_ll_deg> &locations,
    const bpt::time_duration &from, const bpt::time_duration &to, set<feat_desc> &features)
{
    for(vector<geometry::point_ll_deg>::const_iterator itl = locations.begin(); itl != locations.end(); ++itl)
    {
        feat.location = *itl;
        for(bpt::time_duration tdur = from; tdur <= to; tdur += bpt::hours(6))
        {
            feat.reltime = tdur;
            features.insert(feat);
        }
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
set<features_weather::feat_desc> features_weather::get_standard_features(const point_ll_deg &site_location)
{
    // get the closest 4 points of the grid
    const grib_pred_model_gfs gfsmodel;
    const double gridres = gfsmodel.getGridResolution();
    double lonl = static_cast<int>(site_location.lon() / gridres) * gridres;
    double latl = static_cast<int>(site_location.lat() / gridres) * gridres;
    vector<point_ll_deg> nearbyloc;
    nearbyloc.push_back(point_ll_deg(geometry::longitude<>(lonl), geometry::latitude<>(latl)));
    nearbyloc.push_back(point_ll_deg(geometry::longitude<>(lonl + gridres), geometry::latitude<>(latl)));
    nearbyloc.push_back(point_ll_deg(geometry::longitude<>(lonl), geometry::latitude<>(latl + gridres)));
    nearbyloc.push_back(point_ll_deg(geometry::longitude<>(lonl + gridres), geometry::latitude<>(latl + gridres)));

    // parameters and levels
    const array<string, 2> featgnd = {"PRES", "TMP"};
    const array<string, 8> featair = {"HGT", "TMP", "UGRD", "VGRD", "ABSV", "RH", "VVEL", "CLWMR"};
    const array<size_t, 4> levels  = {0, 850, 700, 500};

    feat_desc fdesc;
    fdesc.model = "GFS";
    set<feat_desc> features;
    fdesc.level = 0;
    fdesc.param = featgnd[0];
    generate_features(fdesc, nearbyloc, bpt::hours(-12), bpt::hours(18), features);
    fdesc.param = featgnd[1];
 //   generate_features(fdesc, nearbyloc, bpt::hours(-12), bpt::hours(18), features);

    for(size_t i=1; i<levels.size(); ++i)
    {
        fdesc.level = levels[i];
        for(size_t j=0; j<featair.size(); ++j)
        {
            fdesc.param[j];
            generate_features(fdesc, nearbyloc, bpt::hours(-12), bpt::hours(18), features);
        }
    }

    std::cout << features.size() << " standard features" << std::endl;

    return features;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
struct point_ll_deg_sorter
{
    bool operator()(const point_ll_deg &lhs, const point_ll_deg &rhs)
    {
        if(fabs(lhs.lon()- rhs.lon()) > 0.00001)
        {
            if(lhs.lon() < rhs.lon())
                return true;
            else
                return false;
        }
        if(fabs(lhs.lat()- rhs.lat()) > 0.00001)
        {
            if(lhs.lat() < rhs.lat())
                return true;
            else
                return false;
        }
        return false;
    }
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
vector<double> features_weather::get_features(const set<features_weather::feat_desc> &descriptions, const bgreg::date &day)
{
    static const size_t PG_SIR_WGS84 = 4326;
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "collect weather features");

    vector<point_ll_deg> locations;
    std::transform(descriptions.begin(), descriptions.end(), std::inserter(locations, locations.end()),
        bind(&features_weather::feat_desc::location, _1));
    std::sort(locations.begin(), locations.end(), point_ll_deg_sorter());
    locations.erase(std::unique(locations.begin(), locations.end()), locations.end());
//    for(set<features_weather::feat_desc>::const_iterator it = descriptions.begin(); it != descriptions.end(); ++it)
//        locations.insert(it->location);

    std::stringstream sstr;
    sstr << "SELECT pred_time, AsText(location) as loc, level, parameter, value FROM weather_pred "
         << "WHERE pred_time >= '" << bgreg::to_iso_extended_string(day - bgreg::days(1)) << " 00:00:00' "
         << "AND   pred_time <  '" << bgreg::to_iso_extended_string(day + bgreg::days(2)) << " 00:00:00' "
         << "AND ( ";
    for(vector<point_ll_deg>::const_iterator it = locations.begin(); it != locations.end(); ++it)
    {
        if(it != locations.begin())
            sstr << " OR ";
        sstr << "location = ST_GeomFromText('" << geometry::make_wkt(*it) << "', " << PG_SIR_WGS84 << ") ";
    }
    sstr << ")";
    // todo : we could later restrict the lookup to a radius of 1'000 km around the flying site
//    std::cout << sstr.str() << std::endl;
    pqxx::result res = trans.exec(sstr.str());
//    std::cout << "Query returned " << res.size() << " records." << std::endl;
    if(!res.size())
        throw std::runtime_error("no weather features found.");

    map<feat_desc, double> feat_map;

    for(size_t i=0; i<res.size(); ++i)
    {
        feat_desc currdesc;
        currdesc.model = "GFS";
        string tmpstr;
        res[i]["pred_time"].to(tmpstr);
        bpt::ptime pred_time = bpt::time_from_string(tmpstr);
        currdesc.reltime = pred_time - bpt::ptime(day, bpt::time_duration(0, 0, 0));
        res[i]["loc"].to(tmpstr);
        if(!geometry::from_wkt(tmpstr, currdesc.location))
            throw std::runtime_error("failed to parse the weather location as retured from the database : " + tmpstr);
        res[i]["level"].to(currdesc.level);
        res[i]["parameter"].to(currdesc.param);

        if(descriptions.find(currdesc) != descriptions.end())
        {
            double val;
            res[i]["value"].to(val);
            feat_map[currdesc] = val;
        }
//        else
//            std::cout << "desc not found: " << currdesc << std::endl;
    }

    if(descriptions.size() != feat_map.size())
    {
        std::cout << feat_map.size() << " features found " << descriptions.size() << " expected" << std::endl;
        std::cout << "The following features could not be found in the database for "
                  << bgreg::to_simple_string(day) << " : " << std::endl;
        for(set<feat_desc>::const_iterator it = descriptions.begin(); it != descriptions.end(); ++it)
            if(feat_map.find(*it) == feat_map.end())
                std::cout << (*it) << std::endl;
        throw std::runtime_error("Not all required features found in the database!");
    }

    vector<double> values;
//    std::transform(feat_map.begin(), feat_map.end(), std::back_inserter(values),
//                   boost::bind(&std::pair<feat_desc, double>::second, _1));
    for(map<feat_desc, double>::const_iterator it = feat_map.begin(); it != feat_map.end(); ++it)
        values.push_back(it->second);

    return values;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
bool features_weather::feat_desc::operator==(const features_weather::feat_desc &rhs) const
{
    if(level != rhs.level || model != rhs.model || param != rhs.param)
        return false;
    if((reltime - rhs.reltime).total_seconds())
        return false;
    if(fabs(location.lon() - rhs.location.lon()) > 0.01 || fabs(location.lat() - rhs.location.lat()) > 0.01)
        return false;
    return true;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
bool features_weather::feat_desc::operator<(const features_weather::feat_desc &rhs) const
{
    if(level < rhs.level)
        return true;
    if(level > rhs.level)
        return false;
    if(model < rhs.model)
        return true;
    if(model > rhs.model)
        return false;
    if(param < rhs.param)
        return true;
    if(param > rhs.param)
        return false;
    if(int sec = (reltime - rhs.reltime).total_seconds())
    {
        if(sec > 0)
            return false;
        else
            return true;
    }
    if(fabs(location.lon()- rhs.location.lon()) > 0.00001)
    {
        if(location.lon() < rhs.location.lon())
            return true;
        else
            return false;
    }
    if(fabs(location.lat()- rhs.location.lat()) > 0.00001)
    {
        if(location.lat() < rhs.location.lat())
            return true;
        else
            return false;
    }
    return false;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
std::ostream & flightpred::operator<<(std::ostream &ostr, const features_weather::feat_desc &feat)
{
    ostr << "FEATURE(" << feat.model
         << " " << bpt::to_simple_string(feat.reltime)
         << " " << geometry::make_wkt(feat.location)
         << " " << feat.level
         << " " << feat.param << ")";
    return ostr;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A



