#ifndef GEO_PARSER_H_INCLUDED
#define GEO_PARSER_H_INCLUDED

// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/latlong/latlong.hpp>
// standard library
#include <string>


// the geometry::parse function needs two attributes with lat / lon separately
// we want to be able to provide a single string to parse.
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

boost::geometry::point_ll_deg parse_position(const std::string &pos);

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
}
#endif // GEO_PARSER_H_INCLUDED
