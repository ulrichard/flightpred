// flightpred
#include "main.h"
#include "FlightForecast.h"
#include "WeatherMap.h"
#include "Populations.h"
// pqxx
#include <pqxx/pqxx>
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

//#define _(STRING) gettext(STRING)
#define _(STRING) (STRING)

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
        // assemble the database connection information
        string db_host = "localhost";
        Wt::WApplication::readConfigurationProperty("db-host", db_host);
        string db_port = "5432";
        Wt::WApplication::readConfigurationProperty("db-port", db_port);
        string db_name = "flightpred";
        Wt::WApplication::readConfigurationProperty("db-name", db_name);
        string db_user = "flightpred";
        Wt::WApplication::readConfigurationProperty("db-user", db_user);
        string db_password = "flightpred";
        Wt::WApplication::readConfigurationProperty("db-password", db_password);
        std::stringstream sstr;
        std::cout << "Connecting to the database:  host=" << db_host << " port=" << db_port
                  << " dbname=" << db_name << " user=" << db_user << " password=********" << std::endl;
        sstr << "host=" << db_host << " port=" << db_port << " dbname=" << db_name
             << " user=" << db_user << " password=" << db_password;
        const string db_conn_str = sstr.str();

        // first see if we can connect to the database, and if not put out a good errormessage
        try
        {
            pqxx::connection conn(db_conn_str);
            pqxx::transaction<> trans(conn, "web prediction");
            pqxx::result res = trans.exec("SELECT pred_site_id FROM pred_sites");
        }
        catch(std::exception &ex)
        {
            throw std::runtime_error(string("Failed to connect to the database. You may have to configure the database server, so that the webserver can access it. -> ") + ex.what());
        }

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
        sstr.str("");
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


