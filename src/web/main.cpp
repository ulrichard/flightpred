// flightpred
#include "main.h"
// witty
#include <Wt/WEnvironment>
#include <Wt/WText>
#include <Wt/Ext/TableView>
#include <Wt/WStandardItemModel>
#include <Wt/WDate>
#include <Wt/WTabWidget>
#include <Wt/WContainerWidget>
// pqxx
#include <pqxx/pqxx>
// ggl (boost sandbox)
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/array.hpp>
#include <boost/any.hpp>
// standard libraryhttp://www.google.ch/search?q=dlib+reduce+number+support+vectors&ie=utf-8&oe=utf-8&aq=t&rls=com.ubuntu:en-US:unofficial&client=firefox-a
#include <sstream>
#include <fstream>
#include <utility>

using namespace flightpred;
namespace bgreg = boost::gregorian;
using boost::array;
using boost::any;
using std::string;
using std::vector;
using std::pair;

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
	return Wt::WRun(argc, argv, &createApplication);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// We have a FlightpredApp object for each session (user...)
FlightpredApp::FlightpredApp(const Wt::WEnvironment& env)
 : Wt::WApplication(env)
{
	setTitle("free flight prediction system");               // application title
//	useStyleSheet("flightpred.css");


    // WARNING
    // this is a messy prototype at the moment. It will improve shortly

    try
    {
        // get flight forecasts from the db
        const string db_conn_str = "host=localhost dbname=flightpred user=postgres password=postgres";
        pqxx::connection conn(db_conn_str);
        pqxx::transaction<> trans(conn, "web prediction");
        const bgreg::date today(boost::posix_time::second_clock::universal_time().date());
        const size_t num_fl_lbl = 5;
        const array<string, num_fl_lbl> pred_names = {"num_flight", "max_dist", "avg_dist", "max_dur", "avg_dur"};

        std::stringstream sstr;
        sstr << "SELECT pred_site_id, site_name, AsText(location) as loc FROM pred_sites";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            throw std::invalid_argument("no sites found");

        vector<pair<size_t, string> > sites;
        for(int l=0; l<res.size(); ++l)
        {
            size_t pred_site_id;
            res[l]["pred_site_id"].to(pred_site_id);
            string site_name;
            res[l]["site_name"].to(site_name);
            sites.push_back(std::make_pair(pred_site_id, site_name));
        }

        const size_t forecast_days = 3;
        Wt::WStandardItemModel *model = new Wt::WStandardItemModel(sites.size(), forecast_days);
        for(size_t fdays=0; fdays<forecast_days; ++fdays)
        {
            const bgreg::date day(today + bgreg::days(fdays));
            int yy = day.year();
            int mm = day.month();
            int dd = day.day();
            Wt::WDate wdate(yy, mm, dd);
            model->setHeaderData(fdays, Wt::Horizontal, any(wdate));
        }
        for(size_t i=0; i<pred_names.size(); ++i)
            model->setHeaderData(i, Wt::Vertical, any(pred_names[i]));

        for(vector<pair<size_t, string> >::iterator it = sites.begin(); it != sites.end(); ++it)
        {
            const size_t row = std::distance(sites.begin(), it);

            for(size_t j=0; j<forecast_days; ++j) // days to predict
            {
                const bgreg::date day(today + bgreg::days(j));

                std::stringstream sstr;
                sstr << "SELECT ";
                std::copy(pred_names.begin(), pred_names.end(), std::ostream_iterator<string>(sstr, ", "));
                sstr << "train_sol_id FROM flight_pred WHERE pred_site_id='" << it->first << "' "
                     << "ORDER BY calculated DESC";
                pqxx::result res = trans.exec(sstr.str());
                if(!res.size())
                    continue;

                double max_dist;
                res[0]["max_dist"].to(max_dist);
                model->setData(row, j, any(max_dist));
            }

        }

        Wt::WTabWidget *tabw = new Wt::WTabWidget(root());
        Wt::WContainerWidget *forecastpanel = new Wt::WContainerWidget();
        tabw->addTab(forecastpanel, "Flight Forecast");

        new Wt::WText("FreeFlightPrediction results: ", forecastpanel);

        Wt::Ext::TableView *table = new Wt::Ext::TableView(forecastpanel);
        table->resize(800, 300);
        table->setModel(model);
        table->setColumnSortable(0, true);
        table->setColumnSortable(1, true);
        table->setColumnSortable(2, true);

    }
    catch(std::exception &ex)
    {
        std::cout << "critical error : " << ex.what() << std::endl;
         new Wt::WText(ex.what(), root());
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void FlightpredApp::finalize()
{
    // clean up
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
