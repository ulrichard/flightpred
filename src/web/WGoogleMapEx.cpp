// ggl (boost sandbox)
//#include <boost/geometry/geometry.hpp>
#include <boost/geometry/extensions/gis/latlong/latlong.hpp>
#include <boost/geometry/extensions/gis/io/wkt/stream_wkt.hpp>
#include <boost/geometry/extensions/gis/io/wkt/write_wkt.hpp>
#include <boost/math/constants/constants.hpp>
// witty
#include <Wt/WApplication>
#include "WGoogleMapEx.h"
// boost
#include <boost/foreach.hpp>
// std lib
#include <iostream>
#include <fstream>

using namespace Wt;
using boost::geometry::point_ll_deg;
using boost::geometry::longitude;
using boost::geometry::latitude;
using std::string;


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// the following function is only here as long as it's not part of boost::geometry
namespace boost
{
namespace geometry
{
// Formula to calculate the point at a distance/angle from another point
// This might be a boost::geometry-function in the future.
template <typename P1, typename P2>
inline void point_at_distance(P1 const& p1, double distance, double tc, double radius, P2& p2)
{
    double earth_perimeter = radius * 2.0 * boost::math::constants::pi<double>();
    double d = (distance / earth_perimeter) * 2.0 * boost::math::constants::pi<double>();
    double const& lat1 = boost::geometry::get_as_radian<1>(p1);
    double const& lon1 = boost::geometry::get_as_radian<0>(p1);

    // http://williams.best.vwh.net/avform.htm#LL
    double lat = asin(sin(lat1)*cos(d)+cos(lat1)*sin(d)*cos(tc));
    double dlon = atan2(sin(tc)*sin(d)*cos(lat1),cos(d)-sin(lat1)*sin(lat));
    double lon = lon1 - dlon;

    boost::geometry::set_from_radian<1>(p2, lat);
    boost::geometry::set_from_radian<0>(p2, lon);
}
} // namespace geometry
} // namespace boost
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
WGoogleMapEx::WGoogleMapEx(WContainerWidget *parent)
    : WGoogleMap(parent)
{
    WApplication *app = WApplication::instance();
    app;
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
        const WColor &strokeColor, const size_t strokeWeightPx, const float strokeOpacity)
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

    // workaround as long as we have the problem with the loading order of the javascript -> works only for one map per page
    static const double average_earth_radius = 6372795.0;
    point_ll_deg posi(longitude<>(pos.longitude()), latitude<>(pos.latitude()));
    std::cout << "Circle around " << boost::geometry::make_wkt(posi) << " radius " << radiusKm << " km" << std::endl;
    std::vector<Coordinate> circlepnts;
    const size_t steps = 50;
    for(size_t i=0; i<steps; ++i)
    {
        point_ll_deg perifer;
        point_at_distance(posi, radiusKm * 1000,  2 * M_PI * i / steps, average_earth_radius, perifer);
        circlepnts.push_back(Coordinate(perifer.lat(), perifer.lon()));
    }
    circlepnts.push_back(circlepnts.front());
    assert(circlepnts.size() == steps + 1);
    addPolyline(circlepnts, strokeColor, strokeWeightPx, strokeOpacity);

/*
    // second workaround: draw direcly here without using js lib
    std::ostringstream strm;
    strm << "var svgNS = \"http://www.w3.org/2000/svg\"; "
         << "var circle = document.createElementNS(svgNS, \"svg\"); "
         << "var center = " << jsRef() << ".map.fromLatLngToDivPixel(new google.maps.LatLng(" << pos.latitude() << ", " << pos.longitude() << ")); "
         << "var sz     = " << jsRef() << ".map.getSize(); "
         << "var bnds   = " << jsRef() << ".map.getBounds(); "
         << "var pxDiag = Math.sqrt((sz.width * sz.width) + (sz.height * sz.height)); "
         << "var mDiagKm = bnds.getNorthEast().distanceFrom(bnds.getSouthWest()) / 1000.0; "
         << "var pxPerKm = pxDiag/mDiagKm; "
         << "var w2  = " << strokeWeightPx << "/2.0; "
         << "var rPx = Math.round((" << radiusKm << " * pxPerKm) - w2); "
//         << "var rdrh = circle.suspendRedraw(10000); "
//         << "circle.setAttribute(\"visibility\",\"hidden\"); "
         << "if(rPx > 0 && rPx < 3000) "
         << "{"
         << "    var ne = " << jsRef() << ".map.fromLatLngToDivPixel(bnds.getNorthEast()); "
         << "    var sw = " << jsRef() << ".map.fromLatLngToDivPixel(bnds.getSouthWest()); "
         << "    var wd = ne.x - sw.x; "
         << "    var ht = sw.y - ne.y; "
         << "    var l = sw.x; "
         << "    var t = ne.y; "
         << "    circle.setAttribute(\"width\", wd); "
         << "    circle.setAttribute(\"height\", ht); "
         << "    circle.setAttribute(\"style\", \"position:absolute; top:\"+ t + \"px; left:\" + l + \"px\"); "
         << "    var cx = center.x-l; "
         << "    var cy = center.y-t; "
         << "    circle.setAttribute(\"overflow\", \"hidden\"); "
         << "    circle.setAttribute(\"r\",rPx); "
         << "    circle.setAttribute(\"cx\",cx); "
         << "    circle.setAttribute(\"cy\",cy); "
         << "    circle.setAttribute(\"visibility\",\"visible\"); "
         << "}"
         << jsRef() << ".map.getPane(G_MAP_MAP_PANE).appendChild(circle); ";
//         << "circle.unsuspendRedraw(rdrh); "
//         << "circle.forceRedraw(); ";
    doGmJavaScript(strm.str(), true);

    std::ofstream ofs("/tmp/gmcircle.js");
    ofs << strm.str();
*/
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::addArrow(const Coordinate &pos, const double rotationDeg,
        const WColor &color, const float opacity, const std::string &tooltip)
{
/*
    std::ostringstream strm;
    strm << "var pos = new google.maps.LatLng(" << pos.latitude() << ", " << pos.longitude() << "); "
         << "var arrow = new BDCCArrow(pos, " << rotationDeg << ", \"" << color.cssText() << "\", "
         <<         opacity << ", \"" << tooltip << "\"); "
         << jsRef() << ".map.addOverlay(arrow);";
    doGmJavaScript(strm.str(), true);
*/
    // second attempt

    point_ll_deg posi(longitude<>(pos.longitude()), latitude<>(pos.latitude()));
//    std::cout << "Arrow at " << boost::geometry::make_wkt(posi) << std::endl;
    std::vector<Coordinate> arrowpnts;
    // tail
    point_ll_deg pnt;
    point_at_distance(posi, 20000,  2.0 * M_PI * rotationDeg / 360.0, average_earth_radius, pnt);
    arrowpnts.push_back(Coordinate(pnt.lat(), pnt.lon()));
    // tip
    arrowpnts.push_back(Coordinate(posi.lat(), posi.lon()));
    // right
    point_at_distance(posi, 5000,  2.0 * M_PI * (rotationDeg + 30) / 360.0, average_earth_radius, pnt);
    arrowpnts.push_back(Coordinate(pnt.lat(), pnt.lon()));
    // left
    point_at_distance(posi, 5000,  2.0 * M_PI * (rotationDeg - 30) / 360.0, average_earth_radius, pnt);
    arrowpnts.push_back(Coordinate(pnt.lat(), pnt.lon()));
    // tip
    arrowpnts.push_back(Coordinate(posi.lat(), posi.lon()));

    addPolyline(arrowpnts, color, 2, opacity);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::addWindIndicator(const Coordinate &pos, const double rotationDeg, const double speed,
    const WColor &color, const float opacity, const std::string &tooltip)
{
    const double polelength = 60000;
    const double len10kn    = 10000;
    const double dist10kn   = 10000;

    point_ll_deg posi(longitude<>(pos.longitude()), latitude<>(pos.latitude()));
    std::vector<Coordinate> arrowpnts;
    // tip
    arrowpnts.push_back(Coordinate(posi.lat(), posi.lon()));
    // tail
    point_ll_deg pnt;
    point_at_distance(posi, polelength,  2.0 * M_PI * rotationDeg / 360.0, average_earth_radius, pnt);
    arrowpnts.push_back(Coordinate(pnt.lat(), pnt.lon()));

    for(size_t i=0; i<speed; i+=10)
    {
        const bool full = speed > (i + 5);
        point_at_distance(posi, polelength - i * dist10kn / 10.0,  2.0 * M_PI * rotationDeg / 360.0, average_earth_radius, pnt);
        const point_ll_deg pnt1 = pnt;
        arrowpnts.push_back(Coordinate(pnt1.lat(), pnt1.lon()));
        point_at_distance(pnt1, full ? len10kn : len10kn / 2.0,  2.0 * M_PI * (rotationDeg - 45) / 360.0, average_earth_radius, pnt);
        arrowpnts.push_back(Coordinate(pnt.lat(), pnt.lon()));
        arrowpnts.push_back(Coordinate(pnt1.lat(), pnt1.lon()));
    }

    addPolyline(arrowpnts, color, 2, opacity);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::addText(const Coordinate &pos, const std::string &text, const WColor &color, const float opacity)
{

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
