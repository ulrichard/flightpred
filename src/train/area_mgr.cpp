// flightpred
#include "train/area_mgr.h"
#include "common/geo_parser.h"
#include "common/flightpred_globals.h"
// postgre
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/geometry.hpp>
#include <geometry/geometries/latlong.hpp>
#include <geometry/algorithms/distance.hpp>
#include <geometry/strategies/geographic/geo_distance.hpp>
//#include <geometry/io/wkt/streamwkt.hpp>
#include <geometry/io/wkt/aswkt.hpp>
// boost
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
// standard library
#include <sstream>
#include <algorithm>
#include <utility>
#include <vector>

using namespace flightpred;
using geometry::point_ll_deg;
using boost::lexical_cast;
using boost::tuple;
using boost::make_tuple;
using std::string;
using std::stringstream;
using std::vector;


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
void area_mgr::add_area(const string &name, const point_ll_deg &pos, double area_radius)
{
    pqxx::transaction<> trans(flightpred_db::get_conn(), "manage flight areas");
    vector<tuple<size_t, string, point_ll_deg> > pred_sites;
    stringstream sstr;

    // create or update the site
    if(name.length())
    {
        sstr.str("");
        sstr << "SELECT pred_site_id, AsText(location) AS loc FROM pred_sites WHERE "
             << "site_name='" << name << "'";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
        {
            sstr.str("");
            sstr << "INSERT INTO pred_sites (site_name, location) VALUES ('" << name << "', "
                 << "GeomFromText('" << geometry::make_wkt(pos) << "', " << PG_SIR_WGS84 << "))";
            trans.exec(sstr.str());
        }
        else if(pos.lat() || pos.lon())
        {
            sstr.str("");
            sstr << "UPDATE pred_sites SET location="
                 << "GeomFromText('" << geometry::make_wkt(pos) << "', " << PG_SIR_WGS84 << ") WHERE "
                 << "site_name='" << name << "'";
            trans.exec(sstr.str());
        }
        sstr.str("");
        sstr << "SELECT pred_site_id, site_name, AsText(location) AS loc FROM pred_sites WHERE "
             << "site_name='" << name << "'";
        res = trans.exec(sstr.str());
        if(!res.size())
            throw std::runtime_error("pred site not found!");
        int pred_site_id;
        res[0]["pred_site_id"].to(pred_site_id);
        string site_name;
        res[0]["site_name"].to(site_name);
        string dbloc;
        res[0]["loc"].to(dbloc);
        geometry::point_ll_deg dbpos;
        geometry::from_wkt(dbloc, dbpos);
        pred_sites.push_back(make_tuple(pred_site_id, site_name, dbpos));
    }
    else
    {
        // look or flights in the range for all prediction sites
        sstr.str("");
        sstr << "SELECT pred_site_id, site_name, AsText(location) AS loc FROM pred_sites";
        pqxx::result res = trans.exec(sstr.str());
        for(size_t i=0; i<res.size(); ++i)
        {
            int pred_site_id;
            res[i]["pred_site_id"].to(pred_site_id);
            string site_name;
            res[i]["site_name"].to(site_name);
            string dbloc;
            res[i]["loc"].to(dbloc);
            geometry::point_ll_deg dbpos;
            geometry::from_wkt(dbloc, dbpos);
            pred_sites.push_back(make_tuple(pred_site_id, site_name, dbpos));
        }
    }


    for(vector<tuple<size_t, string, point_ll_deg> >::iterator it = pred_sites.begin(); it != pred_sites.end(); ++it)
    {
        static const double average_earth_radius = 6372795.0;
        geometry::point_ll_deg bbox1, bbox2;
        point_at_distance(it->get<2>(), area_radius * 2.0,  45 * geometry::math::d2r, average_earth_radius, bbox1);
        point_at_distance(it->get<2>(), area_radius * 2.0, 235 * geometry::math::d2r, average_earth_radius, bbox2);

        // todo : move the bounding box points by the radius amount
        sstr.str("");
        sstr << "SELECT site_id, site_name, AsText(location) AS loc FROM sites WHERE "
             << "location && SetSRID('BOX3D(" << bbox1.lon() << " " << bbox1.lat() << ", " << bbox2.lon()
             << " " << bbox2.lat() <<")'::box3d, " << PG_SIR_WGS84 << ")"
             << " AND ST_Distance_Spheroid(ST_GeomFromText('" << it->get<2>() << "', " << PG_SIR_WGS84 << "), "
             << "location, 'SPHEROID[\"WGS84\", 6378137, 298.257223563]') < " << area_radius;
        pqxx::result res = trans.exec(sstr.str());
        std::cout << res.size() << " sites within the search box for " << it->get<1>() << std::endl;
        for(size_t i=0; i<res.size(); ++i)
        {
            size_t site_id;
            res[i]["site_id"].to(site_id);
            string dbloc;
            res[i]["loc"].to(dbloc);
            geometry::point_ll_deg dbpos;
            geometry::from_wkt(dbloc, dbpos);

            // calculate the distance
            const double dist = geometry::distance(it->get<2>(), dbpos, geometry::strategy::distance::vincenty<geometry::point_ll_deg>());
            if(dist <= area_radius)
            {
                sstr.str("");
                sstr << "UPDATE sites SET pred_site_id=" << it->get<0>() << " WHERE site_id=" << site_id;
                trans.exec(sstr.str());
            }
        }
    }



    trans.commit();

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

