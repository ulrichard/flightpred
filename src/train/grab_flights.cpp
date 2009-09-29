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
    flight fl;
    fl.id       = flObj.find("id")->second.get_int();
    fl.pilot    = flObj.find("pilot")->second.get_obj().find("name")->second.get_str();
    fl.takeoff  = flObj.find("takeoff")->second.get_obj().find("name")->second.get_str();
    fl.country  = flObj.find("takeoff")->second.get_obj().find("countryIso")->second.get_str();
    fl.distance = 0.0;
    fl.score    = 0;
    fl.day      = bgreg::from_string(flObj.find("pointStart")->second.get_obj().find("time")->second.get_str().substr(0, 10));

    write_flight_to_db(fl);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void flight_grabber::write_flight_to_db(const flight &fl)
{
    try
    {
        pqxx::connection conn("dbname=flightpred user=postgres");
        pqxx::transaction<> trans(conn, "insert flight");
        // contest
        string sqls = "SELECT contest_id FROM contests WHERE contest_name=\"" + get_contest_name(cont_) + "\"";
        pqxx::result res = trans.exec(sqls);
        size_t contest_id = 0;
        if(res.size())
            res[0]["contest_id"].to(contest_id);
        else
        {
            sqls = "INSERT INTO contests (contest_name) values (\"" + get_contest_name(cont_) + "\")";
            res = trans.exec(sqls);
            contest_id = res.inserted_oid();
        }
        // site
        sqls = "SELECT site_id FROM sites WHERE site_name=\"" + fl.takeoff + "\"";
        res = trans.exec(sqls);
        size_t site_id = 0;
        if(res.size())
            res[0]["site_id"].to(site_id);
        else
        {
            sqls = "INSERT INTO sites (site_name) values (\"" + fl.takeoff + "\")";
            res = trans.exec(sqls);
            site_id = res.inserted_oid();
        }
        // pilot
        sqls = "SELECT pilot_id FROM pilots WHERE pilot_name=\"" + fl.pilot + "\"";
        res = trans.exec(sqls);
        size_t pilot_id = 0;
        if(res.size())
            res[0]["pilot_id"].to(pilot_id);
        else
        {
            sqls = "INSERT INTO pilots (pilot_name) values (\"" + fl.pilot + "\")";
            res = trans.exec(sqls);
            pilot_id = res.inserted_oid();
        }
        // flight
        std::stringstream sstr;
        sstr << "INSERT INTO flights (pilot_id, contest_id, site_id, flight_date, distance, score) "
             << "values (" << pilot_id << ", " << contest_id << ", " << site_id << ", \""
             << bgreg::to_iso_extended_string(fl.day) << "\", " << fl.distance << ", " << fl.score << ")";
        res = trans.exec(sstr.str());
        size_t flight_id = res.inserted_oid();

        trans.commit();

    }
    catch(std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }


}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
