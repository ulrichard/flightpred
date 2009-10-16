#ifndef GRAB_GRIB_H_INCLUDED
#define GRAB_GRIB_H_INCLUDED

// flightpred
#include "grib_pred_model.h"
// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
// boost
#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
// standard library
#include <string>
#include <vector>
#include <map>
#include <set>
#include <ostream>
#include <istream>
#include <utility>

namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class grib_grabber : public boost::noncopyable
{
public:
    grib_grabber(const std::string &db_conn_str, const std::string &model);
    ~grib_grabber() { }

    void grab_grib(const boost::gregorian::date &from, const boost::gregorian::date &to, const geometry::point_ll_deg &pos);
    void download_file(const std::string &url, std::ostream &ostr, std::pair<size_t, size_t> range = std::make_pair(0, 0));
    void read_grib_data(std::istream &istr, const boost::posix_time::ptime &pred_time, size_t level, const std::string &param);

private:
    const std::string db_conn_str_;
    grib_pred_model   *model_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
}; // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // GRAB_GRIB_H_INCLUDED
