
// flightpred
#include "area_mgr.h"
#include "common/train_svm.h"
#include "common/grab_grib.h"
#include "common/grab_flights.h"
#include "common/forecast.h"
#include "common/geo_parser.h"
#include "common/solution_manager.h"
#include "common/flightpred_globals.h"
#include "common/reporter.h"
// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
// boost
#include <boost/program_options.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/foreach.hpp>
// standard library
#include <iostream>
#include <string>

using namespace flightpred;
using namespace flightpred::reporting;
namespace po    = boost::program_options;
namespace bgreg = boost::gregorian;
namespace bfs   = boost::filesystem;
using boost::lexical_cast;
using boost::shared_ptr;
using std::string;
using std::vector;
using std::cout;
using std::endl;


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
vector<string> get_site_names()
{
    vector<string> sites;
    pqxx::transaction<> trans(flightpred_db::get_conn(), "get site names");

    std::stringstream sstr;
    sstr << "SELECT site_name FROM pred_sites";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::invalid_argument("no sites found");
    for(size_t i=0; i<res.size(); ++i)
    {
        string tmpstr;
        res[i][0].to(tmpstr);
        sites.push_back(tmpstr);
    }

    return sites;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
int main(int argc, char* argv[])
{
    try
    {
        string name, position, start_date, end_date, pred_model;
        double area_radius = 5000.0, mutation_rate;
        size_t download_pack = 100, iterations;
        string db_host, db_name, db_user, db_password;
        size_t db_port;

        bgreg::date dtend(bgreg::day_clock::local_day());
        bgreg::date dtstart(dtend - bgreg::months(5));
        start_date = bgreg::to_iso_extended_string(dtstart);
        end_date   = bgreg::to_iso_extended_string(dtend);

        ReportDispatcher::inst().add(new ListenerCout(), reporting::DEBUGING);
        bfs::path error_file(bfs::initial_path() / "error.log");
        ReportDispatcher::inst().add(new ListenerFile(error_file), reporting::ERROR);

        // Declare the supported options.
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help",		       "produce help message")
            ("get-flights",	       "get flights from online competition (name) [import from local files at the moment]")
            ("register-area",      "register a flight area for prediction (name, position, area_radius[km])")
            ("get-weather",        "get past weather prediction grib data around the registered sites (start_date, end_date, download_pack)")
            ("evolve-population",   "evolve a population of solutions (name, generations, mutation_rate)")
            ("train",		       "train the system with the best performing solution found so far (name, start_date, end_date)")
            ("get-future-weather", "get future weather prediction grib data for central europe (download_pack)")
            ("forecast",           "predict possible flights for the next few days")
            ("name",	      po::value<string>(&name)->default_value(""), "name of the area or competition")
            ("position",      po::value<string>(&position)->default_value("N 0 E 0"), "geographic position")
            ("area-radius",   po::value<double>(&area_radius)->default_value(area_radius), "radius around the flight area to include flights for training [m]")
            ("pred-model",    po::value<string>(&pred_model)->default_value("GFS"), "name of the numeric weather prediction model")
            ("start-date",    po::value<string>(&start_date)->default_value(start_date), "start date (yyyy/mm/dd)")
            ("end-date",      po::value<string>(&end_date)->default_value(end_date),     "end date (yyyy/mm/dd)")
            ("download-pack", po::value<size_t>(&download_pack)->default_value(download_pack), "how many grib messages to download at once")
            ("generations",   po::value<size_t>(&iterations)->default_value(50),          "how many generations to run an evolution")
            ("mutation-rate", po::value<double>(&mutation_rate)->default_value(0.125),    "the probability of mutations in the breeding of the evolution")
            ("db-host",       po::value<string>(&db_host)->default_value("localhost"),    "name or ip of the database server")
            ("db-port",       po::value<size_t>(&db_port)->default_value(5432),           "port of the database server")
            ("db-name",       po::value<string>(&db_name)->default_value("flightpred"),   "name of the database")
            ("db-user",       po::value<string>(&db_user)->default_value("postgres"),     "name of the database user")
            ("db-password",   po::value<string>(&db_password)->default_value("postgres"), "password of the database user")
            ;
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        dtstart = bgreg::from_string(start_date);
        dtend   = bgreg::from_string(end_date);
        const string db_conn_str = "host=" + db_host + " port=" + lexical_cast<string>(db_port)
                                 + " dbname=" + db_name + " user=" + db_user + " password=" + db_password;
        flightpred_db::init(db_conn_str);
        const geometry::point_ll_deg pos = parse_position(position);

        const vector<string> site_names = get_site_names();

        bool show_help_msg = true;

        if(vm.count("get-flights"))
        {
            flight_grabber gr(flight_grabber::XCONTEST);
            gr.grab_flights(dtstart, dtend);
            show_help_msg = false;
        }

        if(vm.count("register-area"))
        {
            area_mgr am;
            am.add_area(name, pos, area_radius);
            show_help_msg = false;
        }

        if(vm.count("get-weather"))
        {
            if(pred_model == "GFS")
            {
                grib_grabber_gfs_past gr(download_pack);
                gr.grab_grib(dtstart, dtend);
            }
            else
                throw std::invalid_argument("unknown model: " + pred_model);
            show_help_msg = false;
        }

        if(vm.count("evolve-population"))
        {
            if(name.length())
            {
                solution_manager mgr(name);
                mgr.evolve_population(iterations, mutation_rate);
            }
            else
                BOOST_FOREACH(const string &site_name, site_names)
                {
                    solution_manager mgr(site_name);
                    mgr.evolve_population(iterations, mutation_rate);
                }

            show_help_msg = false;
        }

        if(vm.count("train"))
        {
            train_svm trainer;
            if(name.length())
                trainer.train(name, dtstart, dtend);
            else
                std::for_each(site_names.begin(), site_names.end(),
                    boost::bind(&train_svm::train, boost::ref(trainer), _1, dtstart, dtend));

            show_help_msg = false;
        }

        if(vm.count("get-future-weather"))
        {
            if(pred_model == "GFS")
            {
                grib_grabber_gfs_future gr(download_pack);
                gr.grab_grib(boost::posix_time::hours(24 * 4));
            }
            else
                throw std::invalid_argument("unknown model: " + pred_model);
            show_help_msg = false;
        }

        if(vm.count("forecast"))
        {
            const size_t pred_days = 3;
            forecast fcst;

            if(name.length())
                fcst.prediction_run(name, pred_days);
            else
                std::for_each(site_names.begin(), site_names.end(),
                    boost::bind(&forecast::prediction_run, boost::ref(fcst), _1, pred_days));

            show_help_msg = false;
        }

        if(vm.count("help") || show_help_msg)
        {
            report(WARN) << desc;
            return 1;
        }
    }
    catch(std::exception &ex)
    {
        report(ERROR) << "critical error : " << ex.what();
        return 1;
    }

    return 0;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

