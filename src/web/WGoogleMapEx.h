#ifndef WGOOGLEMAPEX_H_INCLUDED
#define WGOOGLEMAPEX_H_INCLUDED

// witty
#include <Wt/WGoogleMap>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace Wt
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class WGoogleMapEx : public WGoogleMap
{
public:
    WGoogleMapEx(WContainerWidget *parent = 0);
    virtual ~WGoogleMapEx() { }

    /*! \brief Add a marker overlay with a custom icon to the map.
    */
    void addMarker(const Coordinate &pos, const std::string &imgUrl);

    /*! \brief Add a radius overlay to the map. Sorry for the monster interface...
    */
    void addCircle(const Coordinate &pos, const double radiusKm,
        const WColor &strokeColor, const size_t strokeWeightPx, const float strokeOpacity);

    /*! \brief Add an arrow overlay to the map.
    */
    void addArrow(const Coordinate &pos, const double rotationDeg,
        const WColor &color, const float opacity, const std::string &tooltip);

    /*! \brief Add an wind indicator overlay to the map.
    */
    void addWindIndicator(const Coordinate &pos, const double rotationDeg, const double speed,
        const WColor &color, const float opacity, const std::string &tooltip);

    /*! \brief Add some text overlay to the map.
    */
    void addText(const Coordinate &pos, const std::string &text, const WColor &color, const float opacity);


private:
    static const double average_earth_radius = 6372795.0;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace Wt
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // WGOOGLEMAPEX_H_INCLUDED

