
// flightpred
#include "grab_grib.h"
#include "grab_flights.h"
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
	string name, position, start_date, end_date;

    bgreg::date dtend(bgreg::day_clock::local_day());
    bgreg::date dtstart(dtend - bgreg::months(6));
    start_date = bgreg::to_iso_extended_string(dtstart);
    end_date   = bgreg::to_iso_extended_string(dtend);

	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
        ("help",		"produce help message")
        ("get-flights",	"get flights from online competition")
        ("train",		"train the system for an area")
		("name",	    po::value<string>(&name)->default_value("Fiesch"), "name of the area or competition")
		("position",    po::value<string>(&position)->default_value("N 46° 24' 42.96\" O 8° 6' 52.32\""), "geographic position")
		("start_date",  po::value<string>(&start_date)->default_value(start_date), "start date (yyyy/mm/dd)")
		("end_date",    po::value<string>(&end_date)->default_value(end_date),     "end date (yyyy/mm/dd)")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	dtstart = bgreg::from_string(start_date);
	dtend   = bgreg::from_string(end_date);

	if(vm.count("help"))
    {
        cout << desc << "\n";
        return 1;
    }


    if(vm.count("train"))
    {
    // first step : download the grib files

    // second step : load the grib files into the database

    // third step : filter the weather information

    // fourth step : filter the flight information

    // fifth step : train the learning system
    }

    if(vm.count("get-flights"))
    {
        flight_grabber gr;
        gr.grab_flights(flight_grabber::XCONTEST, dtstart, dtend);
    }

    return 0;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

