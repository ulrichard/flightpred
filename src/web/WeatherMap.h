#ifndef WEATHERMAP_H_INCLUDED
#define WEATHERMAP_H_INCLUDED

// witty
#include <Wt/WContainerWidget>
#include <Wt/WCompositeWidget>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace Wt
{
    class WComboBox;
    class WSlider;
    class WGoogleMapEx;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class WeatherMap : public Wt::WCompositeWidget
{
public:
    WeatherMap(const std::string &db_conn_str, const std::string &model_name, Wt::WContainerWidget *parent = 0);
    virtual ~WeatherMap() { }

private:
    void loadWeatherMap();


    Wt::WContainerWidget *impl_;
    const std::string db_conn_str_;
    const std::string model_name_;
    Wt::WComboBox *parameters_;
    Wt::WComboBox *levels_;
    Wt::WSlider   *time_slider_;
    Wt::WGoogleMapEx *gmap_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

#endif // WEATHERMAP_H_INCLUDED
