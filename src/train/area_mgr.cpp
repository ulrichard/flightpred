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
#include <utility>
#include <vector>

using namespace flightpred;
using geometry::point_ll_deg;
using boost::lexical_cast;
using std::string;
using std::stringstream;
using std::vector;
using std::pair;
using std::make_pair;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void area_mgr::add_area(const string &name, const point_ll_deg &pos, double area_radius)
{
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "manage flight areas");
    vector<pair<size_t, point_ll_deg> > pred_sites;
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
        string dbloc;
        res[0]["loc"].to(dbloc);
        geometry::point_ll_deg dbpos;
        geometry::from_wkt(dbloc, dbpos);
        pred_sites.push_back(make_pair(pred_site_id, dbpos));
    }
    else
    {
        // look or flights in the range for all prediction sites
        sstr.str("");
        sstr << "SELECT pred_site_id, AsText(location) AS loc FROM pred_sites";
        pqxx::result res = trans.exec(sstr.str());
        for(size_t i=0; i<res.size(); ++i)
        {
            int pred_site_id;
            res[i]["pred_site_id"].to(pred_site_id);
            string dbloc;
            res[i]["loc"].to(dbloc);
            geometry::point_ll_deg dbpos;
            geometry::from_wkt(dbloc, dbpos);
            pred_sites.push_back(make_pair(pred_site_id, dbpos));
        }
    }


    for(vector<pair<size_t, point_ll_deg> >::iterator it = pred_sites.begin(); it != pred_sites.end(); ++it)
    {
        // search nearby takeoff points
        geometry::point_ll_deg bbox1(it->second), bbox2(it->second);
        geometry::add_value(bbox1, 1.0);
        geometry::subtract_value(bbox2, 1.0);
        // todo : move the bounding box points by the radius amount
        sstr.str("");
        sstr << "SELECT site_id, site_name, AsText(location) AS loc FROM sites WHERE "
             << "location && 'BOX3D(" << bbox1.lat() << " " << bbox1.lon() << " 1.0, " << bbox2.lat()
             << " " << bbox2.lon() <<" -1.0)'::box3d";
        pqxx::result res = trans.exec(sstr.str());
        std::cout << res.size() << " sites within the search box" << std::endl;
        for(size_t i=0; i<res.size(); ++i)
        {
            size_t site_id;
            res[i]["site_id"].to(site_id);
            string dbloc;
            res[i]["loc"].to(dbloc);
            geometry::point_ll_deg dbpos;
            geometry::from_wkt(dbloc, dbpos);

            // calculate the distance
            const double dist = geometry::distance(pos, dbpos, geometry::strategy::distance::vincenty<geometry::point_ll_deg>());
            if(dist <= area_radius)
            {
                sstr.str("");
                sstr << "UPDATE sites SET pred_site_id=" << it->first << " WHERE site_id=" << site_id;
                trans.exec(sstr.str());
            }
        }
    }



    trans.commit();

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
