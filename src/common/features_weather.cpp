// flightpred
#include "common/features_weather.h"
#include "common/grib_pred_model.h"
#include "common/flightpred_globals.h"
#include "common/reporter.h"
#include "common/grab_grib.h"
//#include "area_mgr.h"
// postgre
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
#include <geometry/io/wkt/aswkt.hpp>
#include <geometry/io/wkt/streamwkt.hpp>
#include <geometry/util/graticule.hpp>
#include <geometry/algorithms/distance.hpp>
#include <geometry/strategies/geographic/geo_distance.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
// standard library
#include <vector>
#include <map>
#include <sstream>

using namespace flightpred;
using namespace flightpred::reporting;
using geometry::point_ll_deg;
namespace bgreg = boost::gregorian;
namespace bpt   = boost::posix_time;
using boost::lexical_cast;
using boost::array;
using std::vector;
using std::set;
using std::map;
using std::string;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void features_weather::generate_features(features_weather::feat_desc feat, const vector<point_ll_deg> &locations,
    const bpt::time_duration &from, const bpt::time_duration &to, set<feat_desc> &features) const
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
struct pnt_ll_deg_dist_sorter
{
    pnt_ll_deg_dist_sorter(const point_ll_deg &srch_pnt)
        : srch_pnt_(srch_pnt), strategy_(geometry::strategy::distance::vincenty<point_ll_deg>()) { }

    bool operator()(const point_ll_deg &lhs, const point_ll_deg &rhs)
    {
        const double dist1 = geometry::distance(lhs, srch_pnt_, strategy_);
        const double dist2 = geometry::distance(rhs, srch_pnt_, strategy_);
        return dist1 < dist2;
    }

    const point_ll_deg srch_pnt_;
    const geometry::strategy::distance::vincenty<geometry::point_ll_deg> strategy_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
vector<point_ll_deg> features_weather::get_locations_around_site(const point_ll_deg &site_location, const size_t pnts_per_site, const double gridres)
{
    static boost::unordered_map<point_ll_deg, std::map<double, std::map<size_t, vector<point_ll_deg> > > > cache;
    if(!cache[site_location][gridres][pnts_per_site].empty())
        return cache[site_location][gridres][pnts_per_site];

    vector<point_ll_deg> tmploc;
    double lonl = static_cast<int>(site_location.lon() / gridres) * gridres;
    double latl = static_cast<int>(site_location.lat() / gridres) * gridres;
    for(size_t i=0; i < pnts_per_site; ++i)
        for(size_t j=0; j < pnts_per_site; ++j)
            tmploc.push_back(point_ll_deg(geometry::longitude<>(lonl + (i - pnts_per_site / 2) * gridres),
                                          geometry::latitude<>( latl + (j - pnts_per_site / 2) * gridres)));
    assert(tmploc.size() > pnts_per_site);
    std::sort(tmploc.begin(), tmploc.end(), pnt_ll_deg_dist_sorter(site_location));
    vector<point_ll_deg>::iterator endsel = tmploc.begin();
    std::advance(endsel, pnts_per_site);
    tmploc.erase(endsel, tmploc.end());

    cache[site_location][gridres][pnts_per_site] = tmploc;

    return tmploc;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
features_weather::feat_desc  features_weather::get_random_feature(const geometry::point_ll_deg &site_location)
{
    report(DEBUGING) << "features_weather::get_random_feature(" << /*site_location <<*/ ")";
    feat_desc fd;

    const vector<point_ll_deg> locations_near = get_locations_around_site(site_location,  4, 2.5);
    const vector<point_ll_deg> locations_far  = get_locations_around_site(site_location, 16, 2.5);
    boost::unordered_set<point_ll_deg> locations;
    std::copy(locations_near.begin(), locations_near.end(), std::inserter(locations, locations.end()));
    std::copy(locations_far.begin(),  locations_far.end(),  std::inserter(locations, locations.end()));

    // pick a location
    boost::mt19937 rng;                           // produces randomness out of thin air see pseudo-random number generators
    boost::uniform_int<> distr_loc(0, locations.size()); // distribution that maps to 1..xx see random number distributions
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >  randgen_loc(rng, distr_loc);  // glues randomness with mapping
    boost::unordered_set<point_ll_deg>::const_iterator itloc = locations.begin();
    std::advance(itloc, randgen_loc());
    fd.location = *itloc;
    const bool is_near_location = std::find(locations_near.begin(), locations_near.end(), fd.location) != locations_near.end();

    // pick an altitude level
    const std::set<string> levels = grib_grabber::get_std_levels(is_near_location);
    boost::uniform_int<> distr_lvl(0, levels.size()); // distribution that maps to 1..xx see random number distributions
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >  randgen_lvl(rng, distr_lvl);  // glues randomness with mapping
    std::set<string>::const_iterator itlvl = levels.begin();
    std::advance(itlvl, randgen_lvl());
    fd.level = atoi(itlvl->c_str());

    // pick a parameter
    std::set<string> parameters_air = grib_grabber::get_std_params();
    std::set<string> parameters_gnd;
    parameters_gnd.insert("PRES");
    parameters_gnd.insert("TMP");
    const std::set<string> &param = (fd.level ? parameters_air : parameters_gnd);
    boost::uniform_int<> distr_prm(0, param.size()); // distribution that maps to 1..xx see random number distributions
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >  randgen_prm(rng, distr_prm);  // glues randomness with mapping
    std::set<string>::const_iterator itprm = param.begin();
    std::advance(itprm, randgen_prm());
    fd.param = *itprm;

    // pick a time interval
    boost::uniform_int<> distr_ivr(-2, 3); // distribution that maps to 1..xx see random number distributions
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >  randgen_ivr(rng, distr_ivr);  // glues randomness with mapping
    fd.reltime = bpt::hours(6 * randgen_ivr());

    return fd;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
features_weather::feat_desc  features_weather::mutate_feature(features_weather::feat_desc feat)
{
    report(DEBUGING) << "features_weather::mutate_feature(" << feat << ")";
    feat_desc rand_feat = get_random_feature(feat.location);

    boost::mt19937 rng;
    boost::uniform_int<> distr_mut(0, 4);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >  randgen_mut(rng, distr_mut);

    if(randgen_mut() == 0)
        feat.location = rand_feat.location;

    if(randgen_mut() == 1)
        feat.level = rand_feat.level;

    if(randgen_mut() == 2)
        feat.param = rand_feat.param;

    if(randgen_mut() == 3)
        feat.reltime = rand_feat.reltime;

    return feat;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
set<features_weather::feat_desc> features_weather::get_standard_features(const point_ll_deg &site_location) const
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
    const array<string, 7> featair = {"HGT", "TMP", "UGRD", "VGRD", "ABSV", "RH", "VVEL"}; //, "CLWMR"};
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
            fdesc.param = featair[j];
            generate_features(fdesc, nearbyloc, bpt::hours(-12), bpt::hours(18), features);
        }
    }

    report(VERBOSE).msg(lexical_cast<string>(features.size()) + " standard features");

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
vector<double> features_weather::get_features(const set<features_weather::feat_desc> &descriptions, const bgreg::date &day, const bool future) const
{
    map<feat_desc, double> feat_map;

    if(cached_)
    {
        WeatherFeatureCacheT::const_iterator fitDay = weathercache_.find(day);
        if(fitDay != weathercache_.end())
            BOOST_FOREACH(const feat_desc &fdesc, descriptions)
            {
                map<feat_desc, double>::const_iterator fitFdesc = fitDay->second.find(fdesc);
                if(fitFdesc != fitDay->second.end())
                    feat_map[fdesc] = fitFdesc->second;
            }
    }

    if(descriptions.size() != feat_map.size())
    {

        static const size_t PG_SIR_WGS84 = 4326;
        pqxx::transaction<> trans(flightpred_db::get_conn(), "collect weather features");

        vector<point_ll_deg> locations;
        std::transform(descriptions.begin(), descriptions.end(), std::inserter(locations, locations.end()),
            bind(&features_weather::feat_desc::location, _1));
        std::sort(locations.begin(), locations.end(), point_ll_deg_sorter());
        locations.erase(std::unique(locations.begin(), locations.end()), locations.end());

        const string table_name(future ? "weather_pred_future" : "weather_pred");
        std::stringstream sstr;
        sstr << "SELECT pred_time, AsText(location) as loc, level, parameter, value FROM " << table_name << " "
             << "WHERE pred_time >= '" << bgreg::to_iso_extended_string(day - bgreg::days(1)) << " 00:00:00' "
             << "AND   pred_time <  '" << bgreg::to_iso_extended_string(day + bgreg::days(2)) << " 00:00:00' "
             << "AND   location IN(";
        for(vector<point_ll_deg>::const_iterator it = locations.begin(); it != locations.end(); ++it)
        {
            if(it != locations.begin())
                sstr << ", ";
            sstr << "ST_GeomFromText('" << geometry::make_wkt(*it) << "', " << PG_SIR_WGS84 << ") ";
        }
        sstr << ")";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            throw std::runtime_error("no weather features found.");

        for(size_t i=0; i<res.size(); ++i)
        {
            feat_desc currdesc;
            currdesc.model = "GFS";
            string tmpstr;
            res[i]["pred_time"].to(tmpstr);
            const bpt::ptime pred_time = bpt::time_from_string(tmpstr);
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
        }

        if(descriptions.size() != feat_map.size())
        {
            report(ERROR).msg(lexical_cast<string>(feat_map.size()) + " features found " + lexical_cast<string>(descriptions.size()) + " expected");
            report(WARN)  << "The following features could not be found in the database for "
                          << bgreg::to_simple_string(day) << " : ";
            for(set<feat_desc>::const_iterator it = descriptions.begin(); it != descriptions.end(); ++it)
                if(feat_map.find(*it) == feat_map.end())
                    report(WARN) << (*it);
            throw std::runtime_error("Not all required features found in the database!");
        }

        if(cached_)
        {
            map<feat_desc, double> &daymap = weathercache_[day];
            std::copy(feat_map.begin(), feat_map.end(), std::inserter(daymap, daymap.end()));
        }
    }

    vector<double> values;
//    std::transform(feat_map.begin(), feat_map.end(), std::back_inserter(values),
//                   boost::bind(&std::pair<feat_desc, double>::second, ::_1));
    for(map<feat_desc, double>::const_iterator it = feat_map.begin(); it != feat_map.end(); ++it)
        values.push_back(it->second);

    return values;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
bgreg::date_period features_weather::get_feature_date_period(const bool future_table) const
{
    try
    {
        pqxx::transaction<> trans(flightpred_db::get_conn(), "collect weather features");
        const string table_name(future_table ? "weather_pred_future" : "weather_pred");
        std::stringstream sstr;
        sstr << "SELECT pred_time FROM " << table_name << " " << "ORDER BY pred_time ASC LIMIT 5";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            throw std::runtime_error("no weather features found.");
        string strfrom;
        res[0][0].to(strfrom);
        const bpt::ptime ptfrom = bpt::time_from_string(strfrom);
        sstr.str("");
        sstr << "SELECT pred_time FROM " << table_name << " " << "ORDER BY pred_time DESC LIMIT 5";
        res = trans.exec(sstr.str());
        if(!res.size())
            throw std::runtime_error("no weather features found.");
        string strto;
        res[0][0].to(strto);
        const bpt::ptime ptto = bpt::time_from_string(strto);

        return bgreg::date_period(ptfrom.date() + bgreg::days(1), ptto.date() - bgreg::days(1));
    }
    catch(std::exception&)
    {
        return bgreg::date_period(bgreg::date(2006, 10, 1), bgreg::date(2009, 9, 30));
    }
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
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/*
const vector<double> & features_weather_cached::get_features(const set<features_weather::feat_desc> &descriptions, const bool future)
{
    CacheType::iterator fit = cache_.find(descriptions);
    if(fit != cache_.end())
        return fit->second;

    return cache_.insert(std::make_pair(descriptions, featw_.get_features(descriptions, day_, future))).first->second;
}
*/
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
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



