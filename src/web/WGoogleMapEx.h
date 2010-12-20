#ifndef WGOOGLEMAPEX_H_INCLUDED
#define WGOOGLEMAPEX_H_INCLUDED

// witty
#include <Wt/WGoogleMap>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace Wt
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class WGoogleMapEx : public WCompositeWidget
{
public:
    WGoogleMapEx(WContainerWidget *parent = 0);
    virtual ~WGoogleMapEx() { }

    /*! \brief Sets the map type control.
    *
    * The control allows selecting and switching between supported map types
    * via buttons.
    */
    void setMapTypeControl(WGoogleMap::MapTypeControl type);

    /*! \brief Enables zooming using a mouse's scroll wheel.
    *
    * Scroll wheel zoom is disabled by default.
    */
    void enableScrollWheelZoom();

    /*! \brief Disables zooming using a mouse's scroll wheel.
    *
    * Scroll wheel zoom is disabled by default.
    */
    void disableScrollWheelZoom();

    /*! \brief Enables the dragging of the map (enabled by default).
    */
    void enableDragging();

    /*! \brief Disables the dragging of the map.
    */
    void disableDragging();

    /*! \brief Zooms the map to a region defined by a bounding box.
    */
    void zoomWindow(const std::pair<WGoogleMap::Coordinate, WGoogleMap::Coordinate>& bbox);

    /*! \brief Zooms the map to a region defined by a bounding box.
    */
    void zoomWindow(const WGoogleMap::Coordinate& topLeft, const WGoogleMap::Coordinate& bottomRight);

    /*! \brief Add a marker overlay with a custom icon to the map.
    */
    void addMarker(const WGoogleMap::Coordinate &pos, const std::string &imgUrl);

    /*! \brief Adds a polyline overlay to the map.
    *
    *  specify a value between 0.0 and 1.0 for the opacity.
    */
    void addPolyline(const std::vector<WGoogleMap::Coordinate>& points,
           const WColor& color = red, int width = 2,
           double opacity = 1.0);

    /*! \brief Removes all overlays from the map.
    */
    void clearOverlays();

    /*! \brief Add a radius overlay to the map. Sorry for the monster interface...
    */
    void addCircle(const WGoogleMap::Coordinate &pos, const double radiusKm,
        const WColor &strokeColor, const size_t strokeWeightPx, const float strokeOpacity);

    /*! \brief Add an arrow overlay to the map.
    */
    void addArrow(const WGoogleMap::Coordinate &pos, const double rotationDeg,
        const WColor &color, const float opacity, const std::string &tooltip);

    /*! \brief Add an wind indicator overlay to the map.
    */
    void addWindIndicator(const WGoogleMap::Coordinate &pos, const double rotationDeg, const double speed,
        const WColor &color, const float opacity, const std::string &tooltip);

    /*! \brief Add some text overlay to the map.
    */
    void addText(const WGoogleMap::Coordinate &pos, const std::string &text, const WColor &color, const float opacity);

protected:
    virtual void render(WFlags<RenderFlag> flags);

private:
    std::vector<std::string> additions_;

    virtual void doGmJavaScript(const std::string& jscode, bool sepScope);

    static const double average_earth_radius = 6372795.0;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace Wt
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // WGOOGLEMAPEX_H_INCLUDED

