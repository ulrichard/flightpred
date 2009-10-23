// flightpred
#include "features_weather.h"
// postgre
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
#include <geometry/io/wkt/aswkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
// standard library
#include <vector>
#include <map>
#include <sstream>

using namespace flightpred;
namespace bgreg = boost::gregorian;
namespace bpt   = boost::posix_time;
using std::vector;
using std::set;
using std::map;
using std::string;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
set<features_weather::feat_desc> features_weather::decode_feature_desc(const std::string &desc)
{
    // todo : implement
    set<feat_desc> features;
    return features;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
set<features_weather::feat_desc> features_weather::get_standard_features(const geometry::point_ll_deg &site_location)
{
    // todo : implement
    set<feat_desc> features;
    return features;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
vector<double> features_weather::get_features(const set<features_weather::feat_desc> &descriptions, const bgreg::date &day)
{
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "collect weather features");

    std::stringstream sstr;
    sstr << "SELECT pred_time, AsText(location) as loc, level, parameter, value FROM weather_pred "
         << "WHERE pred_time >= '" << bgreg::to_iso_extended_string(day - bgreg::days(1)) << " 00:00:00' "
         << "AND   pred_time <  '" << bgreg::to_iso_extended_string(day + bgreg::days(2)) << " 00:00:00' ";
    // todo : we could later restrict the lookup to a radius of 1'000 km around the flying site
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("no weather features found.");

    map<feat_desc, double> feat_map;

    for(size_t i=0; i<res.size(); ++i)
    {
        feat_desc currdesc;
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
    }

    if(descriptions.size() != feat_map.size())
    {
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
    return level == rhs.level && model == rhs.model && param == rhs.param
            && reltime == rhs.reltime && location == rhs.location;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
bool features_weather::feat_desc::operator<(const features_weather::feat_desc &rhs) const
{
    return level < rhs.level || model < rhs.model || param < rhs.param
            || reltime < rhs.reltime || location.lat() < rhs.location.lat() || location.lon() < rhs.location.lon();
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



