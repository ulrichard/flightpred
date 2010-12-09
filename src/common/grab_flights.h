#ifndef GRAB_FLIGHTS_H_INCLUDED
#define GRAB_FLIGHTS_H_INCLUDED

// json_spirit
#include <json_spirit_reader.h>
// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/latlong/latlong.hpp>
// boost
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
// standard library
#include <string>


namespace flightpred
{
typedef boost::geometry::model::ll::point<> point_ll_deg;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
struct flight
{
    size_t                          id;
    std::string                     pilot_name;
    std::string                     pilot_country;
    std::string                     takeoff_name;
    std::string                     takeoff_country;
    point_ll_deg                    pos;
    double                          distance;
    double                          score;
    boost::gregorian::date          day;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class flight_grabber : public boost::noncopyable
{
public:
    enum Contest
    {
        OLC,
        XCONTEST
    };

    flight_grabber(Contest cont);
    ~flight_grabber() {}

    static std::string get_contest_name(Contest cont);
    void grab_flights(const boost::gregorian::date &from, const boost::gregorian::date &to);

private:
    void read_json(const boost::filesystem::path &jsonfile);
    void read_flight(const json_spirit::mObject &flObj);
    void write_flight_to_db(const flight &fl);

    const Contest cont_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // GRAB_FLIGHTS_H_INCLUDED
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
