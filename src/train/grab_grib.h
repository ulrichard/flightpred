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
#include <list>
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
    grib_grabber(const std::string &db_conn_str, const std::string &model, size_t download_pack);
    ~grib_grabber() { }

    void grab_grib(const boost::gregorian::date &from, const boost::gregorian::date &to);

private:
    struct request
    {
        boost::posix_time::ptime pred_time;
        size_t level;
        std::string param;
        size_t range_start, range_end; // byte positions in the large grib file on the server
    };
    void download_data(const std::string &url, std::ostream &ostr, const std::list<request> &requests);
    void dispatch_grib_data(std::istream &istr, std::list<request> &requests);
    void read_grib_data(std::istream &istr, const request &req);
    size_t read_until(std::istream &istr, const std::string &srchstr);


    const std::string db_conn_str_;
    grib_pred_model   *model_;
    const size_t       download_pack_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
}; // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // GRAB_GRIB_H_INCLUDED
