#ifndef FEATURES_WEATHER_H_INCLUDED
#define FEATURES_WEATHER_H_INCLUDED

// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
//boost
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/noncopyable.hpp>
// standard library
#include <string>
#include <vector>
#include <set>
#include <ostream>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief Collects the weather features from the database */
class features_weather : public boost::noncopyable
{
public:
    features_weather(bool cached) : cached_(cached) {}
    virtual ~features_weather() {}

    struct feat_desc
    {
        feat_desc() : model("GFS") { }
        feat_desc(const feat_desc &org)
            : model(org.model), reltime(org.reltime), location(org.location), level(org.level), param(org.param) { }
        bool operator==(const feat_desc &rhs) const;
        bool operator<(const feat_desc &rhs) const;

        std::string                      model;
        boost::posix_time::time_duration reltime;
        geometry::point_ll_deg           location;
        size_t                           level;
        std::string                      param;
    };

    std::set<feat_desc> get_standard_features(const geometry::point_ll_deg &site_location) const;

    std::vector<double> get_features(const std::set<feat_desc> &descriptions, const boost::gregorian::date &day, const bool future) const;

    boost::gregorian::date_period get_feature_date_period(const bool future_table) const;

private:
    void generate_features(feat_desc feat, const std::vector<geometry::point_ll_deg> &locations,
        const boost::posix_time::time_duration &from, const boost::posix_time::time_duration &to, std::set<feat_desc> &features) const;

    const bool cached_;
    typedef std::map<boost::gregorian::date, std::map<features_weather::feat_desc, double> > WeatherFeatureCacheT;
    mutable WeatherFeatureCacheT  weathercache_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
std::ostream & operator<<(std::ostream &ostr, const features_weather::feat_desc &feat);
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
};

#endif // FEATURES_WEATHER_H_INCLUDED
