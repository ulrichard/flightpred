// flightpred
#include "WeatherMap.h"
#include <grab_grib.h>
#include <GenGeomLibSerialize.h>

//#if WT_SERIES >= 0x3
// #include "WGoogleMapEx.h"
//#endif

// witty
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WDate>
#include <Wt/WComboBox>
#include <Wt/WSlider>
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WGoogleMap>
// pqxx
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/latlong/latlong.hpp>
// boost
#include <boost/geometry/io/wkt/wkt.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/date_facet.hpp>
#include <boost/array.hpp>
#include <boost/any.hpp>
#include <boost/unordered_map.hpp>
#include <boost/bind.hpp>
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
    parameters_->activated().connect(boost::bind(&WeatherMap::loadWeatherMap, this, false));

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
    levels_->setCurrentIndex(2);
    levels_->activated().connect(boost::bind(&WeatherMap::loadWeatherMap, this, false));

    Wt::WText *txttime = new Wt::WText("Time", impl_);
    time_slider_ = new Wt::WSlider(impl_);
    time_slider_->resize(200, 30);
    time_slider_->setTickInterval(6);
    time_slider_->setMinimum(-12);
    time_slider_->setMaximum(72);
    time_slider_->setValue(18);
    time_slider_->valueChanged().connect(boost::bind(&WeatherMap::loadWeatherMap, this, false));


    new Wt::WBreak(impl_);

    gmap_ = new Wt::WGoogleMap(impl_);
    gmap_->resize(1000, 700);
    gmap_->setMapTypeControl(Wt::WGoogleMap::HierarchicalControl);
    gmap_->enableScrollWheelZoom();
    gmap_->enableDragging();

    loadWeatherMap(true);

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void WeatherMap::loadWeatherMap(const bool resize)
{
    try
    {
        Wt::WApplication::instance()->log("notice") <<  "WeatherMap::loadWeatherMap()";
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
        sstr << "SELECT AsText(location) as loc, value, parameter FROM weather_pred_future "
             << "WHERE model_id=" << model_id << " ";
        if(parameters_->currentText() == "Wind")
            sstr << "AND parameter IN ('UGRD', 'VGRD') ";
        else
            sstr << "AND parameter='TMP' ";
        sstr << "AND level="      << level << " "
             << "AND pred_time='" << bgreg::to_iso_extended_string(when.date()) << " "
             << std::setfill('0') << std::setw(2) << when.time_of_day().hours() << ":00:00' "
             << "ORDER BY run_time ASC";
        res = trans.exec(sstr.str());
        Wt::WApplication::instance()->log("notice") << sstr.str() << "  returned  " << res.size() << " records";

        boost::unordered_map<point_ll_deg, std::map<string, double> > grib_values;
        for(size_t i=0; i<res.size(); ++i)
        {
            string dbloc;
            res[i]["loc"].to(dbloc);
            point_ll_deg dbpos;
            boost::geometry::read_wkt(dbloc, dbpos);
            string param;
            res[i]["parameter"].to(param);
            double val;
            res[i]["value"].to(val);

            grib_values[dbpos][param] = val;
        }


        gmap_->clearOverlays();

        pair<Wt::WGoogleMap::Coordinate, Wt::WGoogleMap::Coordinate> bbox = std::make_pair(Wt::WGoogleMap::Coordinate(90, 180), Wt::WGoogleMap::Coordinate(-90, -180));

        for(boost::unordered_map<point_ll_deg, std::map<string, double> >::iterator it = grib_values.begin(); it != grib_values.end(); ++it)
        {
            const Wt::WGoogleMap::Coordinate gmCoord(it->first.lat(), it->first.lon());

            // bounding box
            bbox.first.setLatitude(  std::min(bbox.first.latitude(),   it->first.lat()));
            bbox.first.setLongitude( std::min(bbox.first.longitude(),  it->first.lon()));
            bbox.second.setLatitude( std::max(bbox.second.latitude(),  it->first.lat()));
            bbox.second.setLongitude(std::max(bbox.second.longitude(), it->first.lon()));


            if(parameters_->currentText() == "Wind")
            {
                const double angle  = atan(it->second["VGRD"] ? it->second["UGRD"] / it->second["VGRD"] : it->second["VGRD"] ? 0 : M_PI);
                const double length = sqrt(it->second["UGRD"] * it->second["UGRD"] + it->second["VGRD"] * it->second["VGRD"]);


//                gmap_->addWindIndicator(gmCoord, angle / 2 / M_PI * 360, length, Wt::WColor("#FF0000"), 0.9, "");

            }
            else
            {
//                gmap_->addMarker(gmCoord, "/sigma.gif");
                gmap_->addMarker(gmCoord);
            }
        }

        if(resize)
        {
            gmap_->zoomWindow(bbox);
                Wt::WApplication::instance()->log("notice") << "WGoogleMap::zoomWindow(" << bbox.first.longitude() << "," << bbox.second.latitude() << " / "
                          << bbox.second.longitude() << "," << bbox.first.latitude() << ")";
        }
    }
    catch(std::exception& ex)
    {
        Wt::WApplication::instance()->log("error") << "Exception in WeatherMap::loadWeatherMap(" << resize << ") : " << ex.what();
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

