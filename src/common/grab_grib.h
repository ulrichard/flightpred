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
    ~grib_grabber() { }

//    virtual void grab_grib(const boost::gregorian::date &from, const boost::gregorian::date &to) = 0;

protected:
    struct request
    {
        boost::posix_time::ptime pred_time;
        size_t level;
        std::string param;
        size_t range_start, range_end; // byte positions in the large grib file on the server
    };
    grib_grabber(const std::string &db_conn_str, const std::string &baseurl, size_t download_pack);
    void download_data(const std::string &url, std::ostream &ostr, const std::list<request> &requests);
    void dispatch_grib_data(std::istream &istr, std::list<request> &requests);

    static std::string get_base_url(const std::string &db_conn_str, const std::string &model, bool future);
    static std::set<std::string> get_std_levels();

    const std::string  db_conn_str_;
    const std::string  baseurl_;
    const size_t       download_pack_;
    static const size_t PG_SIR_WGS84 = 4326;

private:
    void read_grib_data(std::istream &istr, const request &req);
    size_t read_until(std::istream &istr, const std::string &srchstr);
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class grib_grabber_gfs_past : public grib_grabber
{
public:
    grib_grabber_gfs_past(const std::string &db_conn_str, size_t download_pack)
        : grib_grabber(db_conn_str, get_base_url(db_conn_str, "GFS", false), download_pack) { }
    virtual ~grib_grabber_gfs_past() { }

    virtual void grab_grib(const boost::gregorian::date &from, const boost::gregorian::date &to);

};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class grib_grabber_gfs_future : public grib_grabber
{
public:
    grib_grabber_gfs_future(const std::string &db_conn_str, size_t download_pack)
        : grib_grabber(db_conn_str, get_base_url(db_conn_str, "GFS", true), download_pack) { }
    virtual ~grib_grabber_gfs_future() { }

    virtual void grab_grib();

};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
}; // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // GRAB_GRIB_H_INCLUDED
