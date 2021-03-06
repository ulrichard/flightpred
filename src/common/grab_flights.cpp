// flightpred
#include "grab_flights.h"
#include "flightpred_globals.h"
#include "reporter.h"
// libjsoncpp
#include <jsoncpp/json/reader.h>
// postgre
#include <pqxx/pqxx>
// boost
#include <boost/geometry/io/wkt/wkt.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/time_facet.hpp>
#include <boost/timer.hpp>
// standard library
#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace flightpred;
using namespace flightpred::reporting;
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

    // for the moment we only work on testfiles
    const bfs::path testfiledir(bfs::path(__FILE__).parent_path().parent_path().parent_path() / "flights/xcontest_org");

    bfs::directory_iterator end_itr; // default construction yields past-the-end
    for(bfs::directory_iterator itr(testfiledir); itr != end_itr; ++itr)
        if(!bfs::is_directory(itr->status()))
            if(itr->path().extension() == ".json")
                read_json(itr->path());

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void flight_grabber::read_json(const bfs::path &jsonfile)
{
    assert(bfs::exists(jsonfile));
    boost::timer btim;
//    bfs::ifstream is(jsonfile);

    report(INFO) << "Reading json file : " << jsonfile << std::endl;
    Json::Value root;   // will contain the root value after parsing.
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(jsonfile.string(), root);
    if(!parsingSuccessful)
        throw std::runtime_error(("Failed to parse json file" + jsonfile.string()).c_str());
    report(INFO) << "json file read in " << btim.elapsed() << "sec" << std::endl;
    const Json::Value flights = root["items"];
    btim.restart();
    report(INFO) << "start processing " << flights.size() << " records (flights)" << std::endl;
    for(int i=0; i<flights.size(); ++i)
        read_flight(flights[i]);
    report(INFO) << "\nprocessed in " << btim.elapsed() << "sec";
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void flight_grabber::read_flight(const Json::Value &flObj)
{
    flight fl;

    try
    {
        fl.id               = flObj["id"].asInt();
        fl.pilot_name       = flObj["pilot"]["name"].asString();
        fl.pilot_country    = flObj["pilot"]["countryIso"].asString();
        fl.takeoff_name     = flObj["takeoff"]["name"].asString();
        fl.takeoff_country  = flObj["takeoff"]["countryIso"].asString();
        if(!flObj["pointStart"].isNull())
        {
            fl.pos.lat(flObj["pointStart"]["latitude"].asDouble());
            fl.pos.lon(flObj["pointStart"]["longitude"].asDouble());
        }
        else
        {
            string link = flObj["takeoff"]["link"].asString();
            const string srchstr = "filter[point]=";
            const size_t pos = link.find(srchstr);
            if(string::npos == pos)
                throw std::runtime_error("could not evaluate geographic position of takeoff");
            link = link.substr(pos + srchstr.length(), link.length());
            std::stringstream sstr(link);
            double lat, lon;
            sstr >> lon >> lat;
            fl.pos.lon(lon);
            fl.pos.lat(lat);
        }
        fl.distance         = flObj["league"]["route"]["distance"].asDouble();
        fl.score            = flObj["league"]["route"]["points"].asDouble();
        fl.day              = bgreg::from_string(flObj["pointStart"]["time"].asString().substr(0, 10));
        const string durstr = flObj["stats"]["duration"].asString();  // format : "PT01H54M36S"

        std::stringstream sstr(durstr);
        boost::posix_time::time_input_facet* inpfac = new boost::posix_time::time_input_facet();
        sstr.imbue(std::locale(sstr.getloc(), inpfac));
        inpfac->time_duration_format("PT%HH%MM%SS");
        sstr >> fl.duration;

        // the json files contain UTF characters. For the moment, we work only with ASCII in here
        std::transform(fl.pilot_name.begin(),      fl.pilot_name.end(),      fl.pilot_name.begin(),      boost::bind(&flight_grabber::replace_char, ::_1));
        std::transform(fl.pilot_country.begin(),   fl.pilot_country.end(),   fl.pilot_country.begin(),   boost::bind(&flight_grabber::replace_char, ::_1));
        std::transform(fl.takeoff_name.begin(),    fl.takeoff_name.end(),    fl.takeoff_name.begin(),    boost::bind(&flight_grabber::replace_char, ::_1));
        std::transform(fl.takeoff_country.begin(), fl.takeoff_country.end(), fl.takeoff_country.begin(), boost::bind(&flight_grabber::replace_char, ::_1));

        write_flight_to_db(fl);

        static size_t counter = 0;
        if((++counter % 100) == 0)
            std::cout << "|";

    }
    catch(std::exception &ex)
    {
        report(ERROR) << "Exception: " << ex.what() << " with: " << fl.pilot_name << " at " << fl.takeoff_name;
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
char flight_grabber::replace_char(unsigned char cc)
{
    if(cc >= '0' && cc <= '9')
        return cc;
    if(cc >= 'a' && cc <= 'z')
        return cc;
    if(cc >= 'A' && cc <= 'Z')
        return cc;
//  if(cc == 'ä'  || cc == 'ö'  || cc == 'ü'  || cc == 'Ä'  || cc == 'Ö'  || cc == 'Ü')
    if(cc == 0xE4 || cc == 0xF6 || cc == 0xFC || cc == 0xC4 || cc == 0xD6 || cc == 0xDC)  //  ISO 8859-1
        return cc;
    if(cc == '(' || cc == ')' || cc == '-' || cc == '_' || cc == ',' || cc == '.' || cc == '/' || cc == ' ')
        return cc;

    return '_';
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void flight_grabber::write_flight_to_db(const flight &fl)
{
    pqxx::transaction<> trans(flightpred_db::get_conn(), "insert flight");
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
        const size_t PG_SIR_WGS84 = 4326;
        std::stringstream sstr;
        sstr << "INSERT INTO sites (site_name, location, country) values ('"
             << fl.takeoff_name << "', GeomFromText('" << boost::geometry::wkt(fl.pos) << "', "
             << PG_SIR_WGS84 << "), '" << fl.takeoff_country << "') " << "RETURNING site_id";
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
        sstr << "INSERT INTO flights (external_id, pilot_id, contest_id, site_id, flight_date, distance, score, duration) "
             << "values (" << fl.id << "," << pilot_id << ", " << contest_id << ", " << site_id << ", '"
             << bgreg::to_iso_extended_string(fl.day) << "', " << fl.distance << ", " << fl.score << ", " << fl.duration.total_seconds() / 60 << ")";
        res = trans.exec(sstr.str());
    }

    trans.commit();

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
