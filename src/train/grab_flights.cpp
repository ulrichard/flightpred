// flightpred
#include "grab_flights.h"
// postgre
#include <pqxx/pqxx>
// boost
#include <boost/filesystem/fstream.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
// standard library
#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace flightpred;
namespace bfs   = boost::filesystem;
namespace bgreg = boost::gregorian;
using boost::bind;
using std::string;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
flight_grabber::flight_grabber(flight_grabber::Contest cont)
    : cont_(cont)
{

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
string flight_grabber::get_contest_name(Contest cont)
{
    switch(cont)
    {
    case OLC:
        return "onlinecontest";
    case XCONTEST:
        return "xcontest";
    default:
        throw std::invalid_argument("Unknown contest");
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void flight_grabber::grab_flights(const bgreg::date &from, const bgreg::date &to)
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
    try
    {
        flight fl;
        fl.id               = flObj.find("id")->second.get_int();
        fl.pilot_name       = flObj.find("pilot")->second.get_obj().find("name")->second.get_str();
        fl.pilot_country    = flObj.find("pilot")->second.get_obj().find("countryIso")->second.get_str();
        fl.takeoff_name     = flObj.find("takeoff")->second.get_obj().find("name")->second.get_str();
        fl.takeoff_country  = flObj.find("takeoff")->second.get_obj().find("countryIso")->second.get_str();
        string link = flObj.find("takeoff")->second.get_obj().find("link")->second.get_str();
        const string srchstr = "filter[point]=";
        const size_t pos = link.find(srchstr);
        if(string::npos == pos)
            throw std::runtime_error("could not evaluate geographic position of takeoff");
        link = link.substr(pos + srchstr.length(), link.length());
        std::stringstream sstr(link);
        sstr >> fl.lat >> fl.lon;
        fl.distance         = flObj.find("league")->second.get_obj().find("route")->second.get_obj().find("distance")->second.get_real();
        fl.score            = flObj.find("league")->second.get_obj().find("route")->second.get_obj().find("points")->second.get_real();
        fl.day              = bgreg::from_string(flObj.find("pointStart")->second.get_obj().find("time")->second.get_str().substr(0, 10));

        write_flight_to_db(fl);

    }
    catch(std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void flight_grabber::write_flight_to_db(const flight &fl)
{
    pqxx::connection conn("host=localhost dbname=flightpred user=postgres password=postgres");
    pqxx::transaction<> trans(conn, "insert flight");
    // contest
    string sqls = "SELECT contest_id FROM contests WHERE contest_name='" + get_contest_name(cont_) + "'";
    pqxx::result res = trans.exec(sqls);
    size_t contest_id = 0;
    if(res.size())
        res[0]["contest_id"].to(contest_id);
    else
        throw std::runtime_error("contest not registered in db");
    // site
    sqls = "SELECT site_id FROM sites WHERE site_name='" + fl.takeoff_name + "' AND country='" + fl.takeoff_country + "'";
    res = trans.exec(sqls);
    size_t site_id = 0;
    if(res.size())
        res[0]["site_id"].to(site_id);
    else
    {
        std::stringstream sstr;
        sstr << "INSERT INTO sites (site_name, location, country) values ('"
             << fl.takeoff_name << "', point(" << fl.lat << "," << fl.lon << "), '" << fl.takeoff_country << "') "
             << "RETURNING site_id";
        res = trans.exec(sstr.str());
        res[0]["site_id"].to(site_id);
    }
    // pilot
    sqls = "SELECT pilot_id FROM pilots WHERE pilot_name='" + fl.pilot_name + "' AND country='" + fl.pilot_country + "'";
    res = trans.exec(sqls);
    size_t pilot_id = 0;
    if(res.size())
        res[0]["pilot_id"].to(pilot_id);
    else
    {
        std::stringstream sstr;
        sstr << "INSERT INTO pilots (pilot_name, country) values ('"
             << fl.pilot_name << "', '" << fl.pilot_country << "') "
             << "RETURNING pilot_id";
        res = trans.exec(sstr);
        res[0]["pilot_id"].to(pilot_id);
    }
    // flight
    std::stringstream sstr;
    sstr << "SELECT flight_id FROM flights WHERE external_id='" << fl.id << "' AND contest_id='" << contest_id << "'";
    if(!trans.exec(sstr).size())
    {
        std::stringstream sstr;
        sstr << "INSERT INTO flights (external_id, pilot_id, contest_id, site_id, flight_date, distance, score) "
             << "values (" << fl.id << "," << pilot_id << ", " << contest_id << ", " << site_id << ", '"
             << bgreg::to_iso_extended_string(fl.day) << "', " << fl.distance << ", " << fl.score << ")";
        res = trans.exec(sstr.str());
    }

    trans.commit();

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
