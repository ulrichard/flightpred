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
#include <boost/unordered_set.hpp>
// standard library
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <ostream>
#include <istream>
#include <utility>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace geometry
{
    size_t hash_value(const geometry::point_ll_deg &pnt);
    bool operator==(const geometry::point_ll_deg &lhs, const geometry::point_ll_deg &rhs);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
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
    void download_data(const std::string &url, std::ostream &ostr, const std::list<request> &requests, const bool download_last);
    void dispatch_grib_data(std::istream &istr, std::list<request> &requests, const boost::unordered_set<geometry::point_ll_deg> &sel_locations);

    static std::string get_base_url(const std::string &db_conn_str, const std::string &model, bool future);
    static std::set<std::string>     get_std_levels();
    static std::set<std::string>     get_std_params();
    boost::unordered_set<geometry::point_ll_deg> get_locations_around_sites(const double gridres, const size_t pnts_per_site) const;


    const std::string  db_conn_str_;
    const std::string  baseurl_;
    const size_t       download_pack_;
    static const size_t PG_SIR_WGS84 = 4326;

private:
    void read_grib_data(std::istream &istr, const request &req, const boost::unordered_set<geometry::point_ll_deg> &sel_locations);
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
