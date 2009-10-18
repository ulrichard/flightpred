// flightpred
#include "area_mgr.h"
#include "geo_parser.h"
// postgre
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/geometry.hpp>
#include <geometry/algorithms/distance.hpp>
#include <geometry/strategies/geographic/geo_distance.hpp>
// boost
#include <boost/lexical_cast.hpp>
// standard library
#include <sstream>
#include <algorithm>

using namespace flightpred;
using boost::lexical_cast;
using std::string;
using std::stringstream;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void area_mgr::add_area(const string &name, const geometry::point_ll_deg &pos, double area_radius)
{
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "manage flight areas");

    // create or update the site
    stringstream sstr;
    sstr << "SELECT pred_site_id, AsText(location) AS loc FROM pred_sites WHERE "
         << "site_name='" << name << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
    {
        sstr.str("");
        sstr << "INSERT INTO pred_sites (site_name, location) VALUES ('" << name << "', "
             << "GeomFromText('POINT(" << pos.lat() << " " << pos.lon() << ")', -1))";
        trans.exec(sstr.str());
    }
    else if(pos.lat() || pos.lon())
    {
        sstr.str("");
        sstr << "UPDATE pred_sites SET location="
             << "GeomFromText('POINT(" << pos.lat() << " " << pos.lon() << ")', -1)) WHERE"
             << "site_name='" << name << "'";
        trans.exec(sstr.str());
    }
    sstr.str("");
    sstr << "SELECT pred_site_id, AsText(location) AS loc FROM pred_sites WHERE "
         << "site_name='" << name << "'";
    res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("pred site not found!");
    int pred_site_id;
    res[0]["pred_site_id"].to(pred_site_id);



    // search nearby takeoff points
    geometry::point_ll_deg bbox1(pos), bbox2(pos);
    // todo : move the bounding box points by the radius amount
    sstr.str("");
    sstr << "SELECT site_id, site_name, AsText(location) AS loc FROM sites WHERE "
         << "location && 'BOX3D(90900 190900, 100100 200100)'::box3d";
    res = trans.exec(sstr.str());
    for(int i=0; i<res.size(); ++i)
    {
        size_t site_id;
        res[i]["site_id"].to(site_id);
        string dbloc;
        res[i]["loc"].to(dbloc);
        const geometry::point_ll_deg dbpos = parse_position(dbloc);

        // calculate the distance
        const double dist = geometry::distance(pos, dbpos, geometry::strategy::distance::vincenty<geometry::point_ll_deg>());
        if(dist <= area_radius)
        {
            sstr.str("");
            sstr << "UPDATE sites SET pred_site_id=" << pred_site_id << " WHERE site_id=" << site_id;
            trans.exec(sstr.str());
        }
    }



    trans.commit();

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
