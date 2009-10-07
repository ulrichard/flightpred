#ifndef GRAB_GRIB_H_INCLUDED
#define GRAB_GRIB_H_INCLUDED

// flightpred

// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
// boost
#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
// standard library
#include <string>

namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class grib_grabber : public boost::noncopyable
{
public:
    grib_grabber(const std::string &db_conn_str);
    ~grib_grabber() { }

    void grab_grib(const boost::gregorian::date &from, const boost::gregorian::date &to, const geometry::point_ll_deg &pos);
    boost::filesystem::path download_grib(geometry::point_ll_deg &pos);

private:
    const std::string db_conn_str_;
    const std::string grib_fields_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
}; // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // GRAB_GRIB_H_INCLUDED
