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
#include <Wt/WTable>
// pqxx
#include <pqxx/pqxx>
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
        db_conn_str_ = "host=localhost dbname=flightpred user=postgres password=postgres";
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
        tabw->addTab(forecastpanel, "Flight Forecasts");

        const size_t forecast_days = 4;
        Wt::WTable *maintable = new Wt::WTable(forecastpanel);
        for(size_t j=0; j<forecast_days; ++j)
            makePredDay(today + bgreg::days(j), maintable->elementAt(0, j));


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
void FlightpredApp::makePredDay(const bgreg::date &day, Wt::WContainerWidget *parent)
{
    // get flight forecasts from the db
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "web prediction");
    const size_t num_fl_lbl = 5;
    const array<string, num_fl_lbl> pred_names = {"num_flight", "max_dist", "avg_dist", "max_dur", "avg_dur"};

    Wt::WStandardItemModel *model = new Wt::WStandardItemModel(sites_.size(), pred_names.size() + 1);
    model->setHeaderData(0, Wt::Horizontal, any(string("flying site")));
    for(size_t i=0; i<pred_names.size(); ++i)
        model->setHeaderData(i + 1, Wt::Horizontal, any(pred_names[i]));

    for(vector<pair<size_t, string> >::iterator it = sites_.begin(); it != sites_.end(); ++it)
    {
        const size_t row = std::distance(sites_.begin(), it);

        std::stringstream sstr;
        sstr << "SELECT ";
        std::copy(pred_names.begin(), pred_names.end(), std::ostream_iterator<string>(sstr, ", "));
        sstr << "train_sol_id FROM flight_pred WHERE pred_site_id='" << it->first << "' "
             << "AND pred_day='" << bgreg::to_iso_extended_string(day) << "' ORDER BY calculated DESC";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            continue;

        model->setData(row, 0, any(it->second));
        for(size_t i=0; i<pred_names.size(); ++i)
        {
            double val;
            res[0][pred_names[i]].to(val);
            model->setData(row, i + 1, any(val));
        }
    }
    model->sort(1, Wt::DescendingOrder);

    std::stringstream sstr;
    sstr << day;
    new Wt::WText(sstr.str(), parent);


    string renderJs = "function change(val) { return val.toFixed(2); }";

    Wt::Ext::TableView *table = new Wt::Ext::TableView(parent);
    table->resize(400, 300);
    table->setModel(model);
    table->setColumnSortable(0, true);
    table->setColumnWidth(0, 75);
    for(size_t i=0; i<pred_names.size(); ++i)
    {
        table->setColumnSortable(i + 1, true);
        table->setColumnWidth(i + 1, 55);
        table->setRenderer(i + 1, renderJs);
    }

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
