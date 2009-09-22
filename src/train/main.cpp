
// flightpred
#include "grab_grib.h"
// boost
#include <boost/program_options.hpp>
// standard library
#include <iostream>
#include <string>

namespace po  = boost::program_options;
using std::string;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
	string place_name, place_position;

	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
        ("help",		   "produce help message")
		("name",	   po::value<string>(&place_name)->default_value("Fiesch"), "name of the area")
		("position", po::value<string>(&place_position)->default_value("N 46° 24' 42.96\" O 8° 6' 52.32\""), "geographic position")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if(vm.count("help"))
    {
        cout << desc << "\n";
        return 1;
    }


    // first step : download the grib files

    // second step : load the grib files into the database

    // third step : filter the weather information

    // fourth step : filter the flight information

    // fifth step : train the learning system

    return 0;
}
