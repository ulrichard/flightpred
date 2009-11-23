// flightpred
#include "main.h"
#include "WGoogleMapEx.h"
// witty
#include <Wt/WEnvironment>
#include <Wt/WText>
#include <Wt/Ext/TableView>
#include <Wt/WStandardItemModel>
#include <Wt/WDate>
#include <Wt/WTabWidget>
#include <Wt/WContainerWidget>
#include <Wt/WTable>
// pqxx
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
#include <geometry/io/wkt/fromwkt.hpp>
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
// callback function is called everytime when a user enters the page. Can be used to authenticate.
Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
   // You could read information from the environment to decide
   // whether the user has permission to start a new application
    FlightpredApp *flapp = new FlightpredApp(env);

	return flapp;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// main entry point of the application
int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
//    bindtextdomain("flightpred", "/usr/share/locale");
    textdomain("flightpred");

	return Wt::WRun(argc, argv, &createApplication);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// We have a FlightpredApp object for each session (user...)
FlightpredApp::FlightpredApp(const Wt::WEnvironment& env)
 : Wt::WApplication(env)
{
	setTitle(_("free flight prediction system")); // application title
//	useStyleSheet("flightpred.css");
    cout << "create a new session. locale is : " << locale() << endl;


    // WARNING
    // this is a messy prototype at the moment. It will improve shortly

    try
    {
        // get flight forecasts from the db
//        db_conn_str_ = "host=localhost dbname=flightpred user=postgres password=postgres";
        db_conn_str_ = "host=192.168.2.160 port=5432 dbname=flightpred user=postgres password=postgres";
        pqxx::connection conn(db_conn_str_);
        pqxx::transaction<> trans(conn, "web prediction");
        const bgreg::date today(boost::posix_time::second_clock::universal_time().date());
//        const size_t num_fl_lbl = 5;
//        const array<string, num_fl_lbl> pred_names = {"num_flight", "max_dist", "avg_dist", "max_dur", "avg_dur"};

        std::stringstream sstr;
        sstr << "SELECT pred_site_id, site_name, AsText(location) as loc FROM pred_sites";
        pqxx::result res = trans.exec(sstr.str());
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

        Wt::WTabWidget *tabw = new Wt::WTabWidget(root());
        Wt::WContainerWidget *forecastpanel = new Wt::WContainerWidget();
        tabw->addTab(forecastpanel, _("Flight Forecasts\n"));

        const size_t forecast_days = 3;
        Wt::WTable *maintable = new Wt::WTable(forecastpanel);
        for(size_t j=0; j<forecast_days; ++j)
            makePredDay(today + bgreg::days(j), maintable->elementAt(0, j));

        Wt::WContainerWidget *weatherpanel = new Wt::WContainerWidget();
        tabw->addTab(weatherpanel, _("Weather data"));
        showWeatherData(weatherpanel, "GFS", "TMP", 0, bpt::ptime(today, bpt::time_duration(0, 0, 0)));


        Wt::WContainerWidget *docupanel = new Wt::WContainerWidget();
        tabw->addTab(docupanel, _("Documentation"));
        sstr.str("");
        sstr << "<iframe src=\"http://flightpred.svn.sourceforge.net/viewvc/flightpred/trunk/doc/index.html\" width=\"100%\" height=\"100%\"></iframe>";
        Wt::WText *txIFrame = new Wt::WText(sstr.str(), Wt::XHTMLUnsafeText);
        docupanel->addWidget(txIFrame);
        docupanel->resize(Wt::WLength(100.0, Wt::WLength::Percentage), Wt::WLength(100.0, Wt::WLength::Percentage));


    }
    catch(std::exception &ex)
    {
        std::cout << _("critical error : ") << ex.what() << std::endl;
        new Wt::WText(ex.what(), root());
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void FlightpredApp::finalize()
{
    // clean up
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void FlightpredApp::makePredDay(const bgreg::date &day, Wt::WContainerWidget *parent)
{
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
        sstr << "train_sol_id FROM flight_pred WHERE pred_site_id='" << it->first << "' "
             << "AND pred_day='" << bgreg::to_iso_extended_string(day) << "' ORDER BY calculated DESC";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            continue;

        for(size_t i=0; i<pred_names.size(); ++i)
        {
            double val;
            res[0][pred_names[i]].to(val);
            model->setData(row, i + 1, any(val));
        }
    }
    model->sort(1, Wt::DescendingOrder);

    std::stringstream sstr;
    try_imbue(sstr, locale());
//    boost::date_time::date_facet *facet(new boost::date_time::date_facet("%A %x"));
//    sstr.imbue(std::locale(sstr.getloc(), facet));
    sstr << day;
    static const bgreg::date today(boost::posix_time::second_clock::universal_time().date());
    if(day == today)
        sstr << _(" (today)");
    if(day == today + bgreg::days(1))
        sstr << _(" (tomorrow)");
    new Wt::WText(sstr.str(), parent);

    Wt::Ext::TableView *table = new Wt::Ext::TableView(parent);
    table->resize(340, 200);
    table->setModel(model);
    table->setColumnSortable(0, true);
    table->setColumnWidth(0, 75);
    for(size_t i=0; i<pred_names.size(); ++i)
    {
        table->setColumnSortable(i + 1, true);
        table->setColumnWidth(i + 1, i ? 50 : 55);
        table->setRenderer(i + 1, "function change(val) { return val.toFixed(2); }");
    }

#if WT_SERIES >= 0x3

    Wt::WGoogleMapEx *gmap = new Wt::WGoogleMapEx(parent);
    gmap->resize(340, 300);
    gmap->setMapTypeControl(Wt::WGoogleMap::HierarchicalControl);
    gmap->enableScrollWheelZoom();
    gmap->enableDragging();

    pair<Wt::WGoogleMap::Coordinate, Wt::WGoogleMap::Coordinate> bbox = std::make_pair(Wt::WGoogleMap::Coordinate(90, 180), Wt::WGoogleMap::Coordinate(-90, -180));

    for(size_t i=0; i<model->rowCount(); ++i)
    {
        const string site_name = boost::any_cast<string>(model->data(i, 0));

        std::stringstream sstr;
        sstr << "SELECT AsText(location) as loc from pred_sites WHERE site_name='" << site_name << "'";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            continue;

        string dbloc;
        res[0][0].to(dbloc);
        geometry::point_ll_deg dbpos;
        geometry::from_wkt(dbloc, dbpos);

        gmap->addMarker(Wt::WGoogleMap::Coordinate(dbpos.lat(), dbpos.lon()), "/sigma.gif");

        bbox.first.setLatitude(  std::min(bbox.first.latitude(),   dbpos.lat()));
        bbox.first.setLongitude( std::min(bbox.first.longitude(),  dbpos.lon()));
        bbox.second.setLatitude( std::max(bbox.second.latitude(),  dbpos.lat()));
        bbox.second.setLongitude(std::max(bbox.second.longitude(), dbpos.lon()));
    }
    gmap->zoomWindow(bbox);
#endif
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void FlightpredApp::try_imbue(std::ostream &ostr, const string &localename)
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
            cout << "failed to create std::locale for : " << localename << endl;
        }
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void FlightpredApp::showWeatherData(Wt::WContainerWidget *parent, const string &model_name, const string &param, size_t level, const bpt::ptime &when)
{
    // get weather prediction data from the db
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "web prediction");

    std::stringstream sstr;
    sstr << "SELECT model_id from weather_models WHERE model_name='GFS'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("GFS model not found");
    size_t model_id;
    res[0][0].to(model_id);


    sstr.str("");
    sstr << "SELECT AsText(location) as loc, value FROM weather_pred_future "
         << "WHERE model_id=" << model_id << " "
         << "AND parameter='" << param << "' "
         << "AND level="      << level << " "
         << "AND pred_time='" << bgreg::to_iso_extended_string(when.date()) << " "
         << std::setfill('0') << std::setw(2) << when.time_of_day().hours() << ":00:00'";
    res = trans.exec(sstr.str());


#if WT_SERIES >= 0x3



    Wt::WGoogleMapEx *gmap = new Wt::WGoogleMapEx(parent);
    gmap->resize(1000, 700);
    gmap->setMapTypeControl(Wt::WGoogleMap::HierarchicalControl);
    gmap->enableScrollWheelZoom();
    gmap->enableDragging();

    pair<Wt::WGoogleMap::Coordinate, Wt::WGoogleMap::Coordinate> bbox = std::make_pair(Wt::WGoogleMap::Coordinate(90, 180), Wt::WGoogleMap::Coordinate(-90, -180));

    for(size_t i=0; i<res.size(); ++i)
    {
        string dbloc;
        res[i]["loc"].to(dbloc);
        geometry::point_ll_deg dbpos;
        geometry::from_wkt(dbloc, dbpos);
        double val;
        res[i]["value"].to(val);

        gmap->addMarker(Wt::WGoogleMap::Coordinate(dbpos.lat(), dbpos.lon()), "/sigma.gif");

        bbox.first.setLatitude(  std::min(bbox.first.latitude(),   dbpos.lat()));
        bbox.first.setLongitude( std::min(bbox.first.longitude(),  dbpos.lon()));
        bbox.second.setLatitude( std::max(bbox.second.latitude(),  dbpos.lat()));
        bbox.second.setLongitude(std::max(bbox.second.longitude(), dbpos.lon()));
    }
    gmap->zoomWindow(bbox);
#endif
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A


