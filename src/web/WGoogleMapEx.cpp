// witty
#include <Wt/WApplication>
#if WT_SERIES >= 0x3
#include "WGoogleMapEx.h"

using namespace Wt;
using std::string;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
WGoogleMapEx::WGoogleMapEx(WContainerWidget *parent)
    : WGoogleMap(parent)
{
    WApplication *app = WApplication::instance();

    app->require("/googlemaps_addons/BDCCCircle.js");
    app->require("/googlemaps_addons/BDCCArrow.js");
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
    std::ostringstream strm;
    strm << "var pos = new google.maps.LatLng(" << pos.latitude() << ", " << pos.longitude() << "); "
         << "var circle = new BDCCCircle(pos, " << radiusKm << ", \"" << strokeColor.cssText() << "\", "
         <<         strokeWeightPx << ", " << strokeOpacity << ", " << std::boolalpha << doFill
         <<         ", \"" << fillColor.cssText() << "\", " << fillOpacity << ", \"" << tooltip << "\"); "
         << jsRef() << ".map.addOverlay(circle);";

    doGmJavaScript(strm.str(), true);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A


#endif
