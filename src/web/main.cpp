// flightpred
#include "main.h"
#include "FlightForecast.h"
#include "WeatherMap.h"
#include "Populations.h"
// witty
#include <Wt/WEnvironment>
#include <Wt/WText>
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
// gnu gettext
#include <libintl.h>
// standard library
#include <sstream>
#include <iostream>

using namespace flightpred;
using std::string;
using std::vector;

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
	useStyleSheet("flightpred.css");
    std::cout << "create a new session. locale is : " << locale() << std::endl;

    try
    {
        // get flight forecasts from the db
//        const string db_conn_str = "host=localhost dbname=flightpred user=postgres password=postgres";
        const string db_conn_str = "host=192.168.2.160 port=5432 dbname=flightpred user=postgres password=postgres";


        Wt::WTabWidget *tabw = new Wt::WTabWidget(root());

        const size_t forecast_days = 3;
        FlightForecast *forecastpanel = new FlightForecast(db_conn_str, forecast_days);
        tabw->addTab(forecastpanel, _("Flight Forecasts\n"));

        WeatherMap *weatherpanel = new WeatherMap(db_conn_str, "GFS");
        tabw->addTab(weatherpanel, _("Weather data"));

        Populations *populations = new Populations(db_conn_str);
        tabw->addTab(populations, _("Populations"));

        Wt::WContainerWidget *docupanel = new Wt::WContainerWidget();
        tabw->addTab(docupanel, _("Documentation"));
        std::stringstream sstr;
        sstr << "<iframe src=\"http://flightpred.svn.sourceforge.net/viewvc/flightpred/trunk/doc/index.html\" width=\"100%\" height=\"100%\"></iframe>";
        Wt::WText *txIFrame = new Wt::WText(sstr.str(), Wt::XHTMLUnsafeText);
        docupanel->addWidget(txIFrame);
        docupanel->resize(Wt::WLength(100.0, Wt::WLength::Percentage), Wt::WLength(100.0, Wt::WLength::Percentage));

    }
    catch(std::exception &ex)
    {
        std::cout << _("critical error during session initialization : ") << ex.what() << std::endl;
        new Wt::WText(ex.what(), root());
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void FlightpredApp::finalize()
{
    // clean up
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A


