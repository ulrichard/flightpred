
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
	string name, position;

	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
        ("help",		   "produce help message")
        ("get-flights",	   "get flights from online competition")
        ("train",		   "train the system for an area")
		("name",	   po::value<string>(&name)->default_value("Fiesch"), "name of the area or competition")
		("position",   po::value<string>(&position)->default_value("N 46° 24' 42.96\" O 8° 6' 52.32\""), "geographic position")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

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

    }

    return 0;
}
