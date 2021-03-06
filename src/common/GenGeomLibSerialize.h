#ifndef GENGEOMLIBSERIALIZE_H_INCLUDED
#define GENGEOMLIBSERIALIZE_H_INCLUDED

// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/latlong/latlong.hpp>
// boost
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/unordered_set.hpp>


namespace boost
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace serialization
{
    typedef geometry::model::ll::point<> point_ll_deg;

    template <class Archive>
    void save(Archive &ar, const point_ll_deg &pos, const unsigned int version)
    {
		double lat(pos.lat()), lon(pos.lon());
		ar & BOOST_SERIALIZATION_NVP(lat);
		ar & BOOST_SERIALIZATION_NVP(lon);
    }

    template <class Archive>
    void load(Archive &ar, point_ll_deg &pos, const unsigned int version)
    {
		double lat, lon;
		ar & BOOST_SERIALIZATION_NVP(lat);
		ar & BOOST_SERIALIZATION_NVP(lon);
		pos = point_ll_deg(geometry::latitude<>(lat), geometry::longitude<>(lon));
    }

} // namespace serialization
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace geometry
{
    typedef model::ll::point<> point_ll_deg;

    inline size_t hash_value(const point_ll_deg &pnt)
    {
        return boost::hash_value(pnt.lat()) + boost::hash_value(pnt.lon());
    }

    inline bool operator==(const point_ll_deg &lhs, const point_ll_deg &rhs)
    {
        return lhs.lon() == rhs.lon() && lhs.lat() == rhs.lat();
    }
} // namespace geometry

} // namespace boost
BOOST_SERIALIZATION_SPLIT_FREE(point_ll_deg)

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

#endif // GENGEOMLIBSERIALIZE_H_INCLUDED
