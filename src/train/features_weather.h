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



namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class features_weather : public boost::noncopyable
{
public:
    features_weather(const std::string &db_conn_str) : db_conn_str_(db_conn_str) {}
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

    static std::set<feat_desc> decode_feature_desc(const std::string &desc);

    std::set<feat_desc> get_standard_features(const geometry::point_ll_deg &site_location);

    std::vector<double> get_features(const std::set<feat_desc> &descriptions, const boost::gregorian::date &day);

private:
    void generate_features(feat_desc feat, const std::vector<geometry::point_ll_deg> &locations,
        const boost::posix_time::time_duration &from, const boost::posix_time::time_duration &to, std::set<feat_desc> &features);

    const std::string db_conn_str_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
std::ostream & operator<<(std::ostream &ostr, const features_weather::feat_desc &feat);
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
};

#endif // FEATURES_WEATHER_H_INCLUDED
