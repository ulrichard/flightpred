// flightpred
#include "FlightForecast.h"

#if WT_SERIES >= 0x3
 #include "WGoogleMapEx.h"
#endif

// witty
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/Ext/TableView>
#include <Wt/WStandardItemModel>
#include <Wt/WDate>
#include <Wt/WApplication>
#include <Wt/WLogger>
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
// gnu gettext
#include <libintl.h>
// standard library
#include <sstream>
#include <fstream>
#include <utility>

using namespace flightpred;
namespace bgreg = boost::gregorian;
namespace bpt   = boost::posix_time;
using boost::array;
using boost::any;
using std::string;
using std::vector;
using std::pair;
using std::cout;
using std::endl;

#define _(STRING) gettext(STRING)

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
FlightForecast::FlightForecast(const std::string &db_conn_str, const size_t forecast_days, Wt::WContainerWidget *parent)
 : Wt::WCompositeWidget(parent), impl_(new Wt::WContainerWidget()), db_conn_str_(db_conn_str)
{
    setImplementation(impl_);

    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "web prediction");
    const bgreg::date today(boost::posix_time::second_clock::universal_time().date());

    pqxx::result res = trans.exec("SELECT pred_site_id, site_name, AsText(location) as loc FROM pred_sites");
    if(!res.size())
        throw std::invalid_argument("no sites found");

    for(int l=0; l<res.size(); ++l)
    {
        size_t pred_site_id;
        res[l]["pred_site_id"].to(pred_site_id);
        string site_name;
        res[l]["site_name"].to(site_name);
        sites_.push_back(std::make_pair(pred_site_id, site_name));
    }
    trans.commit();

    string showGoogleMapsCount = "1";
    Wt::WApplication::readConfigurationProperty("showGoogleMapsCount", showGoogleMapsCount);
    const size_t gmcount = boost::lexical_cast<size_t>(showGoogleMapsCount);

    Wt::WTable *maintable = new Wt::WTable(impl_);
    maintable->setStyleClass("forecastTable");
    for(size_t j=0; j<forecast_days; ++j)
        makePredDay(today + bgreg::days(j), maintable->elementAt(0, j), maintable->elementAt(1, j), j < gmcount);

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void FlightForecast::makePredDay(const bgreg::date &day, Wt::WContainerWidget *parentForTable, Wt::WContainerWidget *parentForMap, const bool showMap)
{
    try
    {
        Wt::WApplication::instance()->log("notice") <<  "FlightForecast::makePredDay(" << bgreg::to_iso_extended_string(day) << "...)";
        // get flight forecasts from the db
        pqxx::connection conn(db_conn_str_);
        pqxx::transaction<> trans(conn, "web prediction");
        const size_t num_fl_lbl = 5;
        const array<string, num_fl_lbl> pred_names = {"num_flight", "max_dist", "avg_dist", "max_dur", "avg_dur"};

        Wt::WStandardItemModel *model = new Wt::WStandardItemModel(sites_.size(), pred_names.size() + 1);
        model->setHeaderData(0, Wt::Horizontal, any(string(_("flying site"))));
        for(size_t i=0; i<pred_names.size(); ++i)
            model->setHeaderData(i + 1, Wt::Horizontal, any(pred_names[i]));

        for(vector<pair<size_t, string> >::iterator it = sites_.begin(); it != sites_.end(); ++it)
        {
            const size_t row = std::distance(sites_.begin(), it);
            model->setData(row, 0, any(it->second));

            std::stringstream sstr;
            sstr << "SELECT ";
            std::copy(pred_names.begin(), pred_names.end(), std::ostream_iterator<string>(sstr, ", "));
            sstr << "train_sol_id FROM flight_pred WHERE pred_site_id=" << it->first << " "
                 << "AND pred_day='" << bgreg::to_iso_extended_string(day) << "' ORDER BY calculated DESC";
            pqxx::result res = trans.exec(sstr.str());

            if(res.size())
            {
                for(size_t i=0; i<pred_names.size(); ++i)
                {
                    double val;
                    res[0][pred_names[i]].to(val);
                    model->setData(row, i + 1, any(val));
                }
            }
            else
                for(size_t i=0; i<pred_names.size(); ++i)
                    model->setData(row, i + 1, any(-1.0));
        }
        model->sort(1, Wt::DescendingOrder);

        std::stringstream sstr;
        try_imbue(sstr, Wt::WApplication::instance()->locale());
    //    boost::date_time::date_facet *facet(new boost::date_time::date_facet("%A %x"));
    //    sstr.imbue(std::locale(sstr.getloc(), facet));
        sstr << day;
        static const bgreg::date today(boost::gregorian::day_clock::local_day());
        if(day == today)
            sstr << _(" (today)");
        if(day == today + bgreg::days(1))
            sstr << _(" (tomorrow)");
        new Wt::WText(sstr.str(), parentForTable);

        parentForTable->setStyleClass("forecastTableCell");
        Wt::Ext::TableView *table = new Wt::Ext::TableView(parentForTable);
        table->resize(352, 200);
        table->setModel(model);
        table->setColumnSortable(0, true);
        table->setColumnWidth(0, 75);
        for(size_t i=0; i<pred_names.size(); ++i)
        {
            table->setColumnSortable(i + 1, true);
            table->setColumnWidth(i + 1, i ? 50 : 55);
            table->setRenderer(i + 1, "function change(val) { return val.toFixed(2); }");
        }

        if(showMap)
        {
            parentForMap->setStyleClass("forecastTableCell");
            Wt::WGoogleMapEx *gmap = new Wt::WGoogleMapEx(parentForMap);
            gmap->resize(352, 300);
            gmap->setMapTypeControl(Wt::WGoogleMap::HierarchicalControl);
            gmap->enableScrollWheelZoom();
            gmap->enableDragging();

            pair<Wt::WGoogleMap::Coordinate, Wt::WGoogleMap::Coordinate> bbox = std::make_pair(Wt::WGoogleMap::Coordinate(90, 180), Wt::WGoogleMap::Coordinate(-90, -180));

            for(size_t i=0; i<model->rowCount(); ++i)
            {
                const string site_name = boost::any_cast<string>(model->data(i, 0));
                const double max_dist  = boost::any_cast<double>(model->data(i, 2));

                if(max_dist < 0.0)
                    continue;

                std::stringstream sstr;
                sstr << "SELECT AsText(location) as loc from pred_sites WHERE site_name='" << site_name << "'";
                pqxx::result res = trans.exec(sstr.str());
                if(!res.size())
                    continue;

                string dbloc;
                res[0][0].to(dbloc);
                boost::geometry::point_ll_deg dbpos;
                boost::geometry::read_wkt(dbloc, dbpos);

                const Wt::WGoogleMap::Coordinate gmCoord(dbpos.lat(), dbpos.lon());
    //            gmap->addMarker(gmCoord, "/sigma16.gif");
                gmap->addMarker(gmCoord, "/sigma.gif");
                const double radiusKm = max_dist / 10.0;
                gmap->addCircle(gmCoord, radiusKm, Wt::WColor("#FF0000"), 4, 0.9);

                bbox.first.setLatitude(  std::min(bbox.first.latitude(),   dbpos.lat()));
                bbox.first.setLongitude( std::min(bbox.first.longitude(),  dbpos.lon()));
                bbox.second.setLatitude( std::max(bbox.second.latitude(),  dbpos.lat()));
                bbox.second.setLongitude(std::max(bbox.second.longitude(), dbpos.lon()));
            }
            gmap->zoomWindow(bbox);
        }
    }
    catch(std::exception& ex)
    {
        Wt::WApplication::instance()->log("error") << "Exception in FlightForecast::makePredDay() : " << ex.what();
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void FlightForecast::try_imbue(std::ostream &ostr, const string &localename)
{
    try
    {
        ostr.imbue(std::locale(localename.c_str()));
    }
    catch(std::exception &)
    {
        try
        {
            ostr.imbue(std::locale(localename.substr(0, 2).c_str()));
        }
        catch(std::exception &)
        {
            Wt::WApplication::instance()->log("warn") << "failed to create std::locale for : " << localename;
        }
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

