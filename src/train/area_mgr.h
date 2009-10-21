#ifndef AREA_MGR_H_INCLUDED
#define AREA_MGR_H_INCLUDED

// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
// boost
#include <boost/noncopyable.hpp>
// standard library
#include <string>

namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class area_mgr : public boost::noncopyable
{
public:
    area_mgr(const std::string &db_conn_str) : db_conn_str_(db_conn_str) { }
    ~area_mgr() { }

    void add_area(const std::string &name, const geometry::point_ll_deg &pos, double area_radius);
private:
    const std::string db_conn_str_;
    static const size_t PG_SIR_WGS84 = 4326;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // AREA_MGR_H_INCLUDED
