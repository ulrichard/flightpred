// flightpred
#include "WeatherMap.h"
#include "common/grab_grib.h"
#include "common/GenGeomLibSerialize.h"

#if WT_SERIES >= 0x3
 #include "WGoogleMapEx.h"
#endif

// witty
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WDate>
#include <Wt/WComboBox>
#include <Wt/WSlider>
// pqxx
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/latlong/latlong.hpp>
#include <boost/geometry/extensions/gis/io/wkt/read_wkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/date_facet.hpp>
#include <boost/array.hpp>
#include <boost/any.hpp>
#include <boost/unordered_set.hpp>
// gnu gettext
#include <libintl.h>
// standard library
#include <sstream>
#include <fstream>
#include <utility>

using namespace flightpred;
namespace bgreg = boost::gregorian;
namespace bpt   = boost::posix_time;
using boost::geometry::point_ll_deg;
using boost::array;
using boost::any;
using std::string;
using std::vector;
using std::pair;
using std::cout;
using std::endl;

#define _(STRING) gettext(STRING)

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
WeatherMap::WeatherMap(const std::string &db_conn_str, const std::string &model_name, Wt::WContainerWidget *parent)
 : Wt::WCompositeWidget(parent), impl_(new Wt::WContainerWidget()), db_conn_str_(db_conn_str), model_name_(model_name)
{
    setImplementation(impl_);

    Wt::WText *txtparam = new Wt::WText("Parameter", impl_);
    parameters_ = new Wt::WComboBox(impl_);
    parameters_->addItem(_("Wind"));
    parameters_->addItem(_("TMP"));
//    parameters_->addItem(_("Temperature"));
//    parameters_->addItem(_("Humidity"));
    parameters_->setCurrentIndex(0);
    parameters_->activated().connect(SLOT(this, WeatherMap::loadWeatherMap));

    Wt::WText *txtlevel = new Wt::WText("Level", impl_);
    levels_ = new Wt::WComboBox(impl_);
    levels_->addItem(_("GND"));
    levels_->addItem(_("1000 hp"));
    levels_->addItem(_("925 hp"));
    levels_->addItem(_("850 hp"));
    levels_->addItem(_("700 hp"));
    levels_->addItem(_("600 hp"));
    levels_->addItem(_("500 hp"));
    levels_->addItem(_("400 hp"));
    levels_->setCurrentIndex(0);
    levels_->activated().connect(SLOT(this, WeatherMap::loadWeatherMap));

    Wt::WText *txttime = new Wt::WText("Time", impl_);
    time_slider_ = new Wt::WSlider(impl_);
    time_slider_->resize(200, 30);
    time_slider_->setTickInterval(6);
    time_slider_->setMinimum(-12);
    time_slider_->setMaximum(72);
    time_slider_->setValue(18);
    time_slider_->valueChanged().connect(SLOT(this, WeatherMap::loadWeatherMap));


    new Wt::WBreak(impl_);
#if WT_SERIES >= 0x3
    gmap_ = new Wt::WGoogleMapEx(impl_);
    gmap_->resize(1000, 700);
    gmap_->setMapTypeControl(Wt::WGoogleMap::HierarchicalControl);
    gmap_->enableScrollWheelZoom();
    gmap_->enableDragging();
#endif

    loadWeatherMap();

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WeatherMap::loadWeatherMap()
{
    // get weather prediction data from the db
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "web prediction");

    std::stringstream sstr;
    sstr << "SELECT model_id from weather_models WHERE model_name='" << model_name_ << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("model not found");
    size_t model_id;
    res[0][0].to(model_id);


    const size_t level = atoi(levels_->currentText().narrow().c_str());
    const int sliderhr = time_slider_->value() / 6 * 6;
    const bpt::ptime when = bpt::ptime(bgreg::day_clock::local_day(), bpt::time_duration(sliderhr, 0, 0));

    sstr.str("");
    sstr << "SELECT AsText(location) as loc, value FROM weather_pred_future "
         << "WHERE model_id=" << model_id << " ";
    if(parameters_->currentText() == "Wind")
    {
        sstr << "AND (parameter='UGRD' OR parameter='VGRD') ";
    }
    else
        sstr << "AND parameter='" << parameters_->currentText().narrow() << "' ";
    sstr << "AND level="      << level << " "
         << "AND pred_time='" << bgreg::to_iso_extended_string(when.date()) << " "
         << std::setfill('0') << std::setw(2) << when.time_of_day().hours() << ":00:00' "
         << "ORDER BY run_time DESC";
    res = trans.exec(sstr.str());

#if WT_SERIES >= 0x3
    gmap_->clearOverlays();

    pair<Wt::WGoogleMap::Coordinate, Wt::WGoogleMap::Coordinate> bbox = std::make_pair(Wt::WGoogleMap::Coordinate(90, 180), Wt::WGoogleMap::Coordinate(-90, -180));
    boost::unordered_set<point_ll_deg> visited;

    for(size_t i=0; i<res.size(); ++i)
    {
        string dbloc;
        res[i]["loc"].to(dbloc);
        point_ll_deg dbpos;
        boost::geometry::read_wkt(dbloc, dbpos);
        double val;
        res[i]["value"].to(val);
        string param;
        res[i]["parameter"].to(param);

        if(visited.find(dbpos) != visited.end())
            continue;
        visited.insert(dbpos);

        const Wt::WGoogleMap::Coordinate gmCoord(dbpos.lat(), dbpos.lon());

        // bounding box
        bbox.first.setLatitude(  std::min(bbox.first.latitude(),   dbpos.lat()));
        bbox.first.setLongitude( std::min(bbox.first.longitude(),  dbpos.lon()));
        bbox.second.setLatitude( std::max(bbox.second.latitude(),  dbpos.lat()));
        bbox.second.setLongitude(std::max(bbox.second.longitude(), dbpos.lon()));


        if(parameters_->currentText() == "Wind")
        {
            gmap_->addArrow(gmCoord, 0, Wt::WColor("#FF0000"), 0.9, "");

        }
        else
        {
            gmap_->addMarker(gmCoord, "/sigma.gif");
        }
    }
    gmap_->zoomWindow(bbox);
#endif
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

