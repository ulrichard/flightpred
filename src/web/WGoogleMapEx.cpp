// ggl (boost sandbox)
#include <geometry/geometry.hpp>
#include <geometry/geometries/latlong.hpp>
#include <geometry/io/wkt/streamwkt.hpp>
#include <geometry/io/wkt/aswkt.hpp>
// witty
#include <Wt/WApplication>
#if WT_SERIES >= 0x3
#include "WGoogleMapEx.h"
// boost
#include <boost/foreach.hpp>
// std lib
#include <iostream>

using namespace Wt;
using std::string;


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// the following function is only here as long as it's not part of boost::geometry
namespace geometry
{
// Formula to calculate the point at a distance/angle from another point
// This might be a boost::geometry-function in the future.
template <typename P1, typename P2>
inline void point_at_distance(P1 const& p1, double distance, double tc, double radius, P2& p2)
{
    double earth_perimeter = radius * geometry::math::two_pi;
    double d = (distance / earth_perimeter) * geometry::math::two_pi;
    double const& lat1 = geometry::get_as_radian<1>(p1);
    double const& lon1 = geometry::get_as_radian<0>(p1);

    // http://williams.best.vwh.net/avform.htm#LL
    double lat = asin(sin(lat1)*cos(d)+cos(lat1)*sin(d)*cos(tc));
    double dlon = atan2(sin(tc)*sin(d)*cos(lat1),cos(d)-sin(lat1)*sin(lat));
    double lon = lon1 - dlon;

    geometry::set_from_radian<1>(p2, lat);
    geometry::set_from_radian<0>(p2, lon);
}
} // namespace geometry
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
WGoogleMapEx::WGoogleMapEx(WContainerWidget *parent)
    : WGoogleMap(parent)
{
    WApplication *app = WApplication::instance();
/*
    // if there is no google api key configured, use the one for
    // http://localhost:8080/
    static const string localhost_key
    = "ABQIAAAAWqrN5o4-ISwj0Up_depYvhTwM0brOpm-"
      "All5BF6PoaKBxRWWERS-S9gPtCri-B6BZeXV8KpT4F80DQ";

    string googlekey = localhost_key;
    Wt::WApplication::readConfigurationProperty("google_api_key", googlekey);

    // init the google javascript api
    const string gmuri = "http://www.google.com/jsapi?key=" + googlekey;
    app->require(gmuri, "google");
*/
//    app->require("/googlemaps_addons/BDCCArrow.js",  "GM_Arrow");
//    app->require("/googlemaps_addons/BDCCCircle.js", "GM_Circle");

//    app->require("http://www.bdcc.co.uk/Gmaps/BDCCCircle.js");
//    app->require("http://www.bdcc.co.uk/Gmaps/BDCCArrow.js");

/*
    std::stringstream sstr;
    sstr << "function onGoogleApiLoad()"
         << "{"
         <<     "var scriptnode1  = document.createElement('SCRIPT');"
         <<     "scriptnode1.type = 'text/javascript';"
         <<     "scriptnode1.src  = '/googlemaps_addons/BDCCCircle.js';"
         <<     "var scriptnode2  = document.createElement('SCRIPT');"
         <<     "scriptnode2.type = 'text/javascript';"
         <<     "scriptnode2.src  = '/googlemaps_addons/BDCCArrow.js';"
         <<     "var headnode = document.getElementsByTagName('HEAD');"
         <<     "if(headnode[0] != null)"
         <<     "{"
         <<         "headnode[0].appendChild(scriptnode1);"
         <<         "headnode[0].appendChild(scriptnode2);"
         <<     "}"
         << "};"
         << "google.setOnLoadCallback(onGoogleApiLoad);";
    app->doJavaScript(sstr.str(), true);
*/
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::addMarker(const Coordinate &pos, const string &imgUrl)
{
    std::ostringstream strm;
    strm << "var icon = new google.maps.Icon(G_DEFAULT_ICON);"
         << "icon.image = \"" << imgUrl << "\";"
         << "var marker = new google.maps.Marker(new google.maps.LatLng("
         << pos.latitude() << ", " << pos.longitude() << "), icon);"
         << jsRef() << ".map.addOverlay(marker);";

    doGmJavaScript(strm.str(), false);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::addCircle(const Coordinate &pos, const double radiusKm,
        const WColor &strokeColor, const size_t strokeWeightPx, const float strokeOpacity, const bool doFill,
        const WColor &fillColor,   const float fillOpacity, const std::string &tooltip)
{
/*
    std::ostringstream strm;
    strm << "var pos = new google.maps.LatLng(" << pos.latitude() << ", " << pos.longitude() << "); "
         << "var circle = new BDCCCircle(pos, " << radiusKm << ", \"" << strokeColor.cssText() << "\", "
         <<         strokeWeightPx << ", " << strokeOpacity << ", " << std::boolalpha << doFill
         <<         ", \"" << fillColor.cssText() << "\", " << fillOpacity << ", \"" << tooltip << "\"); "
         << jsRef() << ".map.addOverlay(circle);";

    doGmJavaScript(strm.str(), true);
*/
    // workaround as long as we have the problem with the loading order of the javascript
    static const double average_earth_radius = 6372795.0;
    geometry::point_ll_deg posi(geometry::longitude<>(pos.longitude()), geometry::latitude<>(pos.latitude()));
    std::cout << "Circle around " << geometry::make_wkt(posi) << std::endl;
    std::vector<Coordinate> circlepnts;
    const size_t steps = 50;
    for(size_t i=0; i<steps; ++i)
    {
        geometry::point_ll_deg perifer;
        point_at_distance(posi, radiusKm * 1000,  2 * M_PI * i / steps, average_earth_radius, perifer);
        circlepnts.push_back(Coordinate(perifer.lat(), perifer.lon()));
    }
    circlepnts.push_back(circlepnts.front());
    assert(circlepnts.size() == steps + 1);
    addPolyline(circlepnts, strokeColor, strokeWeightPx, strokeOpacity);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::addArrow(const Coordinate &pos, const double rotationDeg,
        const WColor &color, const float opacity, const std::string &tooltip)
{
    std::ostringstream strm;
    strm << "var pos = new google.maps.LatLng(" << pos.latitude() << ", " << pos.longitude() << "); "
         << "var arrow = new BDCCArrow(pos, " << rotationDeg << ", \"" << color.cssText() << "\", "
         <<         opacity << ", \"" << tooltip << "\"); "
         << jsRef() << ".map.addOverlay(arrow);";

    doGmJavaScript(strm.str(), true);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A


#endif
