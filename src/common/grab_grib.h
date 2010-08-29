#ifndef GRAB_GRIB_H_INCLUDED
#define GRAB_GRIB_H_INCLUDED

// flightpred
#include "grib_pred_model.h"
// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/latlong/latlong.hpp>
// boost
#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>
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
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class grib_grabber : public boost::noncopyable
{
public:
    ~grib_grabber() { }

//    virtual void grab_grib(const boost::gregorian::date &from, const boost::gregorian::date &to) = 0;

    static std::set<std::string>     get_std_levels(const bool full);
    static std::set<std::string>     get_std_params();

protected:
    struct request
    {
        boost::posix_time::ptime pred_time;
        size_t level;
        std::string param;
        size_t range_start, range_end; // byte positions in the large grib file on the server
    };
    grib_grabber(const std::string &modelname, size_t download_pack, const bool is_future, const bool ignore_errors);
    void download_data(const std::string &url, std::ostream &ostr, const std::list<request> &requests);
    void dispatch_grib_data(std::istream &istr, std::list<request> &requests,
            const boost::unordered_set<boost::geometry::point_ll_deg> &sel_locations_close,
            const boost::unordered_set<boost::geometry::point_ll_deg> &sel_locations_wide);

    static std::string get_base_url(const std::string &model, bool future);
    static size_t      get_model_id(const std::string &model);
    static double      get_grid_res(const std::string &model);
    boost::unordered_set<boost::geometry::point_ll_deg> get_locations_around_sites(const double gridres, const size_t pnts_per_site) const;

    const std::string        baseurl_;
    const std::string        modelname_;
    const size_t             db_model_id_;
    const double             grid_res_;
    const size_t             download_pack_;
    const bool               is_future_;
    const bool               ignore_errors_;
    static const size_t      PG_SIR_WGS84 = 4326;
    boost::posix_time::ptime predrun_;

#ifdef _DEBUG
    boost::unordered_map<boost::geometry::point_ll_deg, boost::tuple<size_t, size_t, size_t> > grid_point_stats_; // position / num_missing / num_skipped / num_selected
#endif
private:
    void read_grib_data(std::istream &istr, const request &req,
            const boost::unordered_set<boost::geometry::point_ll_deg> &sel_locations_close,
            const boost::unordered_set<boost::geometry::point_ll_deg> &sel_locations_wide);
    size_t read_until(std::istream &istr, const std::string &srchstr);
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class grib_grabber_gfs_past : public grib_grabber
{
public:
    grib_grabber_gfs_past(size_t download_pack, bool ignore_errors)
        : grib_grabber("GFS", download_pack, false, ignore_errors) { }
    virtual ~grib_grabber_gfs_past() { }

    virtual void grab_grib(const boost::gregorian::date &from, const boost::gregorian::date &to);

};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class grib_grabber_gfs_future : public grib_grabber
{
public:
    grib_grabber_gfs_future(size_t download_pack)
        : grib_grabber("GFS", download_pack, true, false) { }
    virtual ~grib_grabber_gfs_future() { }

    virtual void grab_grib(const boost::posix_time::time_duration &future_time);

};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/*
class grib_grabber_gfs_OPeNDAP : public grib_grabber
{
public:
    grib_grabber_gfs_OPeNDAP(const std::string &db_conn_str)
        : grib_grabber("GFS", 1, true) { db_conn_str; } // "nomads.ncep.noaa.gov:9090/dods/gfs/"
    virtual ~grib_grabber_gfs_OPeNDAP() { }

    virtual void grab_grib(const boost::posix_time::time_duration &future_time);
protected:
    void read_ascii_data(std::istream &istr);
};
*/
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // GRAB_GRIB_H_INCLUDED
