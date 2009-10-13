
// flightpred
#include "grab_grib.h"
#include "grab_flights.h"
#include "geo_parser.h"
// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
// boost
#include <boost/program_options.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
// standard library
#include <iostream>
#include <string>

using namespace flightpred;
namespace po    = boost::program_options;
namespace bgreg = boost::gregorian;
using std::string;
using std::cout;
using std::endl;


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
int main(int argc, char* argv[])
{
    try
    {
        string name, position, start_date, end_date, pred_model;
        string db_host, db_name, db_user, db_password;

        bgreg::date dtend(bgreg::day_clock::local_day());
        bgreg::date dtstart(dtend - bgreg::months(6));
        start_date = bgreg::to_iso_extended_string(dtstart);
        end_date   = bgreg::to_iso_extended_string(dtend);

        // Declare the supported options.
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help",		"produce help message")
            ("get-flights",	"get flights from online competition")
            ("get-weather", "get weather prediction grib data")
            ("train",		"train the system for an area")
            ("name",	    po::value<string>(&name)->default_value("Fiesch"), "name of the area or competition")
            ("position",    po::value<string>(&position)->default_value("N 46 24 42.96 E 8 6 52.32"), "geographic position")
            ("pred_model",  po::value<string>(&pred_model)->default_value("GFS"), "name of the numeric weather prediction model")
            ("start_date",  po::value<string>(&start_date)->default_value(start_date), "start date (yyyy/mm/dd)")
            ("end_date",    po::value<string>(&end_date)->default_value(end_date),     "end date (yyyy/mm/dd)")
            ("db_host",     po::value<string>(&db_host)->default_value("localhost"), "name or ip of the database server")
            ("db_name",     po::value<string>(&db_name)->default_value("flightpred"), "name of the database")
            ("db_user",     po::value<string>(&db_user)->default_value("postgres"), "name of the database user")
            ("db_password", po::value<string>(&db_password)->default_value("postgres"), "password of the database user")
            ;
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        dtstart = bgreg::from_string(start_date);
        dtend   = bgreg::from_string(end_date);
        const string db_conn_str = "host=" + db_host + " dbname=" + db_name + " user=" + db_user + " password=" + db_password;
        const geometry::point_ll_deg &pos = parse_position(position);

        if(vm.count("help"))
        {
            cout << desc << "\n";
            return 1;
        }

        if(vm.count("get-flights"))
        {
            flight_grabber gr(flight_grabber::XCONTEST, db_conn_str);
            gr.grab_flights(dtstart, dtend);
        }

        if(vm.count("get-weather"))
        {
            grib_grabber gr(db_conn_str, pred_model);
            gr.grab_grib(dtstart, dtend, pos);
        }


        if(vm.count("train"))
        {
        // first step : download the grib files

        // second step : load the grib files into the database

        // third step : filter the weather information

        // fourth step : filter the flight information

        // fifth step : train the learning system
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

