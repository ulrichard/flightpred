// ggl (boost sandbox)
//#include <boost/geometry/geometry.hpp>
#include <boost/geometry/extensions/gis/latlong/latlong.hpp>
#include <boost/geometry/extensions/gis/io/wkt/stream_wkt.hpp>
#include <boost/geometry/extensions/gis/io/wkt/write_wkt.hpp>
#include <boost/math/constants/constants.hpp>
// witty
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include "WGoogleMapEx.h"
// boost
#include <boost/foreach.hpp>
// std lib
#include <iostream>
#include <fstream>

using namespace Wt;
using boost::geometry::longitude;
using boost::geometry::latitude;
using std::string;

typedef boost::geometry::model::ll::point<> point_ll_deg;

namespace {
  // if there is no google api key configured, use the one for
  // http://localhost:8080/
  static const std::string localhost_key
    = "ABQIAAAAWqrN5o4-ISwj0Up_depYvhTwM0brOpm-"
      "All5BF6PoaKBxRWWERS-S9gPtCri-B6BZeXV8KpT4F80DQ";
}

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
{
    setImplementation(new WContainerWidget());

    WApplication *app = WApplication::instance();

    std::string googlekey = localhost_key;
    Wt::WApplication::readConfigurationProperty("google_api_key", googlekey);

    // init the google javascript api
    const std::string gmuri = "http://www.google.com/jsapi?key=" + googlekey;
    app->require(gmuri, "google");

    if(parent)
        parent->addWidget(this);
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
void WGoogleMapEx::render(WFlags<RenderFlag> flags)
{
  if (flags & RenderFull) {
    // initialize the map
    std::stringstream strm;
    strm <<
      "{ function initialize() {"
      """var self = " << jsRef() << ";"
      """var map = new google.maps.Map2(self);"
      """map.setCenter(new google.maps.LatLng(47.01887777, 8.651888), 13);"
      """self.map = map;"
/*
      // eventhandling
      """google.maps.Event.addListener(map, \"click\", "
      ""                              "function(overlay, latlng) {"
      ""  "if (latlng) {"
      ""  << clicked_.createCall("latlng.lat() +' '+ latlng.lng()") << ";"
      ""  "}"
      """});"

      """google.maps.Event.addListener(map, \"dblclick\", "
      ""                              "function(overlay, latlng) {"
      ""  "if (latlng) {"
      ""  << doubleClicked_.createCall("latlng.lat() +' '+ latlng.lng()") << ";"
      ""  "}"
      """});"

      """google.maps.Event.addListener(map, \"mousemove\", "
      ""                              "function(latlng) {"
      ""  "if (latlng) {"
      ""  << mouseMoved_.createCall("latlng.lat() +' '+ latlng.lng()") << ";"
      ""  "}"
*/
      """});";

    // additional things
    for (unsigned int i = 0; i < additions_.size(); i++)
      strm << additions_[i];

    strm <<
      "}" // function initialize()
      "google.load(\"maps\", \"2\", "
      ""          "{other_params:\"sensor=false\", callback: initialize});"
      "}"; // private scope

    additions_.clear();

    WApplication::instance()->doJavaScript(strm.str());
  }

  WCompositeWidget::render(flags);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::clearOverlays()
{
  doGmJavaScript(jsRef() + ".map.clearOverlays();", false);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::doGmJavaScript(const std::string& jscode, bool sepScope)
{
  std::string js = jscode;
  // to keep the variables inside a scope where they don't interfere
  if (sepScope)
    js = "{" + js + "}";

  if (isRendered())
    WApplication::instance()->doJavaScript(js);
  else
    additions_.push_back(js);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::addMarker(const WGoogleMap::Coordinate &pos, const string &imgUrl)
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
void WGoogleMapEx::addCircle(const WGoogleMap::Coordinate &pos, const double radiusKm,
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
    std::vector<WGoogleMap::Coordinate> circlepnts;
    const size_t steps = 50;
    for(size_t i=0; i<steps; ++i)
    {
        point_ll_deg perifer;
        point_at_distance(posi, radiusKm * 1000,  2 * M_PI * i / steps, average_earth_radius, perifer);
        circlepnts.push_back(WGoogleMap::Coordinate(perifer.lat(), perifer.lon()));
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
void WGoogleMapEx::addArrow(const WGoogleMap::Coordinate &pos, const double rotationDeg,
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
    std::vector<WGoogleMap::Coordinate> arrowpnts;
    // tail
    point_ll_deg pnt;
    point_at_distance(posi, 20000,  2.0 * M_PI * rotationDeg / 360.0, average_earth_radius, pnt);
    arrowpnts.push_back(WGoogleMap::Coordinate(pnt.lat(), pnt.lon()));
    // tip
    arrowpnts.push_back(WGoogleMap::Coordinate(posi.lat(), posi.lon()));
    // right
    point_at_distance(posi, 5000,  2.0 * M_PI * (rotationDeg + 30) / 360.0, average_earth_radius, pnt);
    arrowpnts.push_back(WGoogleMap::Coordinate(pnt.lat(), pnt.lon()));
    // left
    point_at_distance(posi, 5000,  2.0 * M_PI * (rotationDeg - 30) / 360.0, average_earth_radius, pnt);
    arrowpnts.push_back(WGoogleMap::Coordinate(pnt.lat(), pnt.lon()));
    // tip
    arrowpnts.push_back(WGoogleMap::Coordinate(posi.lat(), posi.lon()));

    addPolyline(arrowpnts, color, 2, opacity);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::addWindIndicator(const WGoogleMap::Coordinate &pos, const double rotationDeg, const double speed,
    const WColor &color, const float opacity, const std::string &tooltip)
{
    const double polelength = 60000;
    const double len10kn    = 10000;
    const double dist10kn   = 10000;

    point_ll_deg posi(longitude<>(pos.longitude()), latitude<>(pos.latitude()));
    std::vector<WGoogleMap::Coordinate> arrowpnts;
    // tip
    arrowpnts.push_back(WGoogleMap::Coordinate(posi.lat(), posi.lon()));
    // tail
    point_ll_deg pnt;
    point_at_distance(posi, polelength,  2.0 * M_PI * rotationDeg / 360.0, average_earth_radius, pnt);
    arrowpnts.push_back(WGoogleMap::Coordinate(pnt.lat(), pnt.lon()));

    for(size_t i=0; i<speed; i+=10)
    {
        const bool full = speed > (i + 5);
        point_at_distance(posi, polelength - i * dist10kn / 10.0,  2.0 * M_PI * rotationDeg / 360.0, average_earth_radius, pnt);
        const point_ll_deg pnt1 = pnt;
        arrowpnts.push_back(WGoogleMap::Coordinate(pnt1.lat(), pnt1.lon()));
        point_at_distance(pnt1, full ? len10kn : len10kn / 2.0,  2.0 * M_PI * (rotationDeg - 45) / 360.0, average_earth_radius, pnt);
        arrowpnts.push_back(WGoogleMap::Coordinate(pnt.lat(), pnt.lon()));
        arrowpnts.push_back(WGoogleMap::Coordinate(pnt1.lat(), pnt1.lon()));
    }

    addPolyline(arrowpnts, color, 2, opacity);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::addText(const WGoogleMap::Coordinate &pos, const std::string &text, const WColor &color, const float opacity)
{

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::setMapTypeControl(WGoogleMap::MapTypeControl type)
{
    std::string control;

    switch(type)
    {
    case WGoogleMap::DefaultControl:
        control = "google.maps.MapTypeControl";
        break;
    case WGoogleMap::MenuControl:
        control = "google.maps.MenuMapTypeControl";
        break;
    case WGoogleMap::HierarchicalControl:
        control = "google.maps.HierarchicalMapTypeControl";
        break;
    default:
        control = "";
    }

    std::stringstream strm;
    strm << jsRef() << ".map.removeControl(" << jsRef() << ".mtc);";

    if(control != "")
        strm << "var mtc = new " << control << "();"
             << jsRef() << ".mtc = mtc;"
             << jsRef() << ".map.addControl(mtc);";

    doGmJavaScript(strm.str(), false);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::enableScrollWheelZoom()
{
    doGmJavaScript(jsRef() + ".map.enableScrollWheelZoom();", false);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::disableScrollWheelZoom()
{
    doGmJavaScript(jsRef() + ".map.disableScrollWheelZoom();", false);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::enableDragging()
{
    doGmJavaScript(jsRef() + ".map.enableDragging();", false);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::disableDragging()
{
    doGmJavaScript(jsRef() + ".map.disableDragging();", false);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::zoomWindow(const std::pair<WGoogleMap::Coordinate, WGoogleMap::Coordinate>& bbox)
{
  zoomWindow(bbox.first, bbox.second);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::zoomWindow(const WGoogleMap::Coordinate& topLeft, const WGoogleMap::Coordinate& rightBottom)
{
  WGoogleMap::Coordinate topLeftC = topLeft;
  WGoogleMap::Coordinate rightBottomC = rightBottom;

  const WGoogleMap::Coordinate center
    ((topLeftC.latitude() + rightBottomC.latitude()) / 2.0,
     (topLeftC.longitude() + rightBottomC.longitude()) / 2.0);

  topLeftC = WGoogleMap::Coordinate(std::min(topLeftC.latitude(), rightBottomC.latitude()),
                                    std::min(topLeftC.longitude(), rightBottomC.longitude()));
  rightBottomC = WGoogleMap::Coordinate(std::max(topLeftC.latitude(), rightBottomC.latitude()),
                                        std::max(topLeftC.longitude(), rightBottomC.longitude()));
  std::stringstream strm;
  strm << "var bbox = new google.maps.LatLngBounds(new google.maps.LatLng("
       << topLeftC.latitude()  << ", " << topLeftC.longitude() << "), "
       << "new google.maps.LatLng("
       << rightBottomC.latitude() << ", " << rightBottomC.longitude() << "));"
       << "var zooml = " << jsRef() << ".map.getBoundsZoomLevel(bbox);"
       << jsRef() << ".map.setCenter(new google.maps.LatLng("
       << center.latitude() << ", " << center.longitude() << "), zooml);";

  doGmJavaScript(strm.str(), true);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WGoogleMapEx::addPolyline(const std::vector<WGoogleMap::Coordinate>& points,
			     const WColor& color, int width, double opacity)
{
  // opacity has to be between 0.0 and 1.0
  opacity = std::max(std::min(opacity, 1.0), 0.0);

  std::stringstream strm;
  strm << "var waypoints = [];";
  for (size_t i = 0; i < points.size(); ++i)
    strm << "waypoints[" << i << "] = new google.maps.LatLng("
	 << points[i].latitude() << ", " << points[i].longitude() << ");";

  strm << "var poly = new google.maps.Polyline(waypoints, \""
       << color.cssText() << "\", " << width << ", " << opacity << ");"
       << jsRef() << ".map.addOverlay(poly);";

  doGmJavaScript(strm.str(), true);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
