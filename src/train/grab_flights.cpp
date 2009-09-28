// flightpred
#include "grab_flights.h"
// boost
#include <boost/filesystem/fstream.hpp>
#include <boost/bind.hpp>
// standard library
#include <iostream>

using namespace flightpred;
namespace bfs   = boost::filesystem;
namespace bgreg = boost::gregorian;
using boost::bind;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
flight_grabber::flight_grabber()
{

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void flight_grabber::grab_flights(flight_grabber::Contest cont, const bgreg::date &from, const bgreg::date &to)
{
    const bfs::path testfile(bfs::path(__FILE__).parent_path().parent_path().parent_path() / "flights/xcontest_org/flights.sample.json");

    read_json(testfile);

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void flight_grabber::read_json(const bfs::path &jsonfile)
{
    assert(bfs::exists(jsonfile));
    bfs::ifstream is(jsonfile);
    json_spirit::mValue val;
    json_spirit::read(is, val);

    const json_spirit::mArray flights = val.get_obj().find("items")->second.get_array();
//    std::for_each(flights.begin(), flights.end(),
//        boost::bind(&flight_grabber::read_flight, boost::bind(&json_spirit::mValue::get_obj, ::_1)));
    for(json_spirit::mArray::const_iterator it = flights.begin(); it != flights.end(); ++it)
        read_flight(it->get_obj());
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void flight_grabber::read_flight(const json_spirit::mObject &flObj)
{
    flight fl;
    fl.id       = flObj.find("id")->second.get_int();
    fl.pilot    = flObj.find("pilot")->second.get_obj().find("name")->second.get_str();
    fl.takeoff  = flObj.find("takeoff")->second.get_obj().find("name")->second.get_str();
    fl.country  = flObj.find("takeoff")->second.get_obj().find("countryIso")->second.get_str();
    fl.distance = 0.0;
    fl.score    = 0;

    write_flight_to_db(fl);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void flight_grabber::write_flight_to_db(const flight &fl)
{

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
