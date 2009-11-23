#ifndef WGOOGLEMAPEX_H_INCLUDED
#define WGOOGLEMAPEX_H_INCLUDED

// witty
#include <Wt/WGoogleMap>

namespace Wt
{
class WGoogleMapEx : public WGoogleMap
{
public:
    WGoogleMapEx(WContainerWidget *parent = 0) : WGoogleMap(parent) { }
    virtual ~WGoogleMapEx() { }

    /*! \brief Add a marker overlay to the map.
    */
    void addMarker(const Coordinate &pos, const std::string url);

private:

};
} // namespace Wt
#endif // WGOOGLEMAPEX_H_INCLUDED
