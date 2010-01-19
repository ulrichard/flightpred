#ifndef GENGEOMLIBSERIALIZE_H_INCLUDED
#define GENGEOMLIBSERIALIZE_H_INCLUDED

// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
// boost
#include <boost/serialization/split_free.hpp>


namespace boost
{
namespace serialization
{

    template <class Archive>
    void save(Archive &ar, const geometry::point_ll_deg &pos, const unsigned int version)
    {
		double lat(pos.lat()), lon(pos.lon());
		ar & BOOST_SERIALIZATION_NVP(lat);
		ar & BOOST_SERIALIZATION_NVP(lon);
    }

    template <class Archive>
    void load(Archive &ar, geometry::point_ll_deg &pos, const unsigned int version)
    {
		double lat, lon;
		ar & BOOST_SERIALIZATION_NVP(lat);
		ar & BOOST_SERIALIZATION_NVP(lon);
		pos = geometry::point_ll_deg(geometry::latitude<>(lat), geometry::longitude<>(lon));
    }

} // namespace serialization
} // namespace boost
BOOST_SERIALIZATION_SPLIT_FREE(geometry::point_ll_deg)


#endif // GENGEOMLIBSERIALIZE_H_INCLUDED
