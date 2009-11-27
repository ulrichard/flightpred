
// flightpred
#include "area_mgr.h"
#include "train_svm.h"
#include "common/grab_grib.h"
#include "common/grab_flights.h"
#include "common/forecast.h"
#include "common/geo_parser.h"
// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
// boost
#include <boost/program_options.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
// standard library
#include <iostream>
#include <string>

using namespace flightpred;
namespace po    = boost::program_options;
namespace bgreg = boost::gregorian;
using boost::lexical_cast;
using std::string;
using std::cout;
using std::endl;


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
int main(int argc, char* argv[])
{
    try
    {
        string name, position, start_date, end_date, pred_model;
        double area_radius = 5000.0;
        size_t download_pack = 100;
        string db_host, db_name, db_user, db_password;
        size_t db_port;

        bgreg::date dtend(bgreg::day_clock::local_day());
        bgreg::date dtstart(dtend - bgreg::months(5));
        start_date = bgreg::to_iso_extended_string(dtstart);
        end_date   = bgreg::to_iso_extended_string(dtend);

        // Declare the supported options.
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help",		       "produce help message")
            ("get-flights",	       "get flights from online competition (name) [import from local files at the moment]")
            ("register-area",      "register a flight area for prediction (name, position, area_radius[km])")
            ("get-weather",        "get past weather prediction grib data for central europe (start_date, end_date, download_pack)")
            ("init-population",    "create an initial generation 0 of solutions (name)")
            ("evolution-single",   "evolve a single generation (name)")
            ("evolution-converge", "run the evolution until the progress falls below a threshold (name)")
            ("train",		       "train the system (name, start_date, end_date)")
            ("get-future-weather", "get future weather prediction grib data for central europe (download_pack)")
            ("forecast",           "predict possible flights for the next few days")
            ("name",	      po::value<string>(&name)->default_value(""), "name of the area or competition")
            ("position",      po::value<string>(&position)->default_value("N 0 E 0"), "geographic position")
            ("area_radius",   po::value<double>(&area_radius)->default_value(area_radius), "radius around the flight area to include flights for training [m]")
            ("pred_model",    po::value<string>(&pred_model)->default_value("GFS"), "name of the numeric weather prediction model")
            ("start_date",    po::value<string>(&start_date)->default_value(start_date), "start date (yyyy/mm/dd)")
            ("end_date",      po::value<string>(&end_date)->default_value(end_date),     "end date (yyyy/mm/dd)")
            ("download_pack", po::value<size_t>(&download_pack)->default_value(download_pack), "how many grib messages to download at once")
            ("db_host",       po::value<string>(&db_host)->default_value("localhost"),    "name or ip of the database server")
            ("db_port",       po::value<size_t>(&db_port)->default_value(5432),           "port of the database server")
            ("db_name",       po::value<string>(&db_name)->default_value("flightpred"),   "name of the database")
            ("db_user",       po::value<string>(&db_user)->default_value("postgres"),     "name of the database user")
            ("db_password",   po::value<string>(&db_password)->default_value("postgres"), "password of the database user")
            ;
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        dtstart = bgreg::from_string(start_date);
        dtend   = bgreg::from_string(end_date);
        const string db_conn_str = "host=" + db_host + " port=" + lexical_cast<string>(db_port)
                                 + " dbname=" + db_name + " user=" + db_user + " password=" + db_password;
        const geometry::point_ll_deg pos = parse_position(position);


        bool show_help_msg = true;

        if(vm.count("get-flights"))
        {
            flight_grabber gr(flight_grabber::XCONTEST, db_conn_str);
            gr.grab_flights(dtstart, dtend);
            show_help_msg = false;
        }

        if(vm.count("register-area"))
        {
            area_mgr am(db_conn_str);
            am.add_area(name, pos, area_radius);
            show_help_msg = false;
        }

        if(vm.count("get-weather"))
        {
            if(pred_model == "GFS")
            {
                grib_grabber_gfs_past gr(db_conn_str, download_pack);
                gr.grab_grib(dtstart, dtend);
            }
            else
                throw std::invalid_argument("unknown model: " + pred_model);
            show_help_msg = false;
        }

        if(vm.count("init-population"))
        {


            show_help_msg = false;
        }

        if(vm.count("evolution-single"))
        {

            show_help_msg = false;
        }

        if(vm.count("evolution-converge"))
        {

            show_help_msg = false;
        }

        if(vm.count("train"))
        {
            train_svm trainer(db_conn_str);
            if(name.length())
                trainer.train(name, dtstart, dtend);
            else
                trainer.train_all(dtstart, dtend);
            show_help_msg = false;
        }

        if(vm.count("get-future-weather"))
        {
            if(pred_model == "GFS")
            {
                grib_grabber_gfs_future gr(db_conn_str, download_pack);
                gr.grab_grib(boost::posix_time::hours(24 * 4));
            }
            else
                throw std::invalid_argument("unknown model: " + pred_model);
            show_help_msg = false;
        }

        if(vm.count("forecast"))
        {
            forecast fcst(db_conn_str);
            fcst.prediction_run(3);
            show_help_msg = false;
        }

        if(vm.count("help") || show_help_msg)
        {
            cout << desc << "\n";
            return 1;
        }
    }
    catch(std::exception &ex)
    {
        std::cout << "critical error : " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

