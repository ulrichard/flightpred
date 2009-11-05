// flightpred
#include "grab_grib.h"
#include "geo_parser.h"
// grib api
#include "grib_api.h"
// postgre
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/geometry.hpp>
#include <geometry/io/wkt/aswkt.hpp>
#include <geometry/io/wkt/fromwkt.hpp>
#include <geometry/algorithms/distance.hpp>
#include <geometry/strategies/geographic/geo_distance.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/asio.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/foreach.hpp>
// standard library
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <numeric>


using namespace flightpred;
using geometry::point_ll_deg;
namespace bfs = boost::filesystem;
namespace bgreg = boost::gregorian;
namespace bpt = boost::posix_time;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using boost::lexical_cast;
using boost::asio::ip::tcp;
using namespace boost::lambda;
using std::string;
using std::vector;
using std::list;
using std::map;
using std::set;
using std::pair;
using std::make_pair;
using std::ios_base;
using std::cout;
using std::endl;
using std::stringstream;

// a description of the grib data fields can be found at:
// http://www.nco.ncep.noaa.gov/pmb/products/gfs/gfs.t00z.pgrb2f00.shtml
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
grib_grabber::grib_grabber(const std::string &db_conn_str, const std::string &baseurl, size_t download_pack)
    : db_conn_str_(db_conn_str), baseurl_(baseurl), download_pack_(download_pack)
{

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
string grib_grabber::get_base_url(const std::string &db_conn_str, const string &model, bool future)
{
    pqxx::connection conn(db_conn_str);
    pqxx::transaction<> trans(conn, "get model details");

    std::stringstream sstr;
    sstr << "SELECT grid_step, url_past, url_future FROM weather_models where model_name ='" << model << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("No details found in the db for weather prediction model " + model);
    string url;
    res[0][future ? "url_future" : "url_past"].to(url);
    return url;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber::download_data(const string &url, std::ostream &ostr, const list<request> &requests, const bool download_last)
{
    const list<request>::const_iterator requend = (download_last ? requests.end() : --requests.end());

    cout << "downloading : " << url << " with " << std::distance(requests.begin(), requend) << " byte ranges totaling "
         << std::accumulate(requests.begin(), requend, 0,
               _1 + bind(&request::range_end, _2) - bind(&request::range_start, _2)) / 1024.0
         << " kBytes" << endl;

    boost::asio::io_service io_service;

    size_t pos = url.find('/');
    if(pos == string::npos)
        throw std::invalid_argument("invalid url");
    const string hostname = url.substr(0, pos);
    const string path     = url.substr(pos, url.length());

    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(hostname, "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    // Try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while(error && endpoint_iterator != end)
    {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
    }
    if(error)
        throw boost::system::system_error(error);


    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << path << " HTTP/1.1\r\n";
    request_stream << "Host: " << hostname << "\r\n";
    request_stream << "Accept: */*\r\n";
    if(std::distance(requests.begin(), requend))
    {
        request_stream << "Range: bytes=";
        for(list<grib_grabber::request>::const_iterator it = requests.begin(); it != requend; ++it)
        {
            if(it->range_start >= it->range_end || !it->range_end)
                continue;
            if(it != requests.begin())
                request_stream << ",";
            request_stream << it->range_start << "-";
            if(it->range_end)
                request_stream << it->range_end - 1;
        }
        request_stream << "\r\n";
    }
    request_stream << "Connection: close\r\n\r\n";

    // Send the request.
    boost::asio::write(socket, request);

    // Read the response status line.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if(!response_stream || http_version.substr(0, 5) != "HTTP/")
        throw std::runtime_error("Invalid response");
    if(status_code != 200 && status_code != 206)
        throw std::runtime_error("Response returned with status code " + lexical_cast<string>(status_code));

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response, "\r\n\r\n");

    // Process the response headers.
    string header;
    while(std::getline(response_stream, header) && header != "\r")
        cout << header << "\n";
    cout << "\n";

    // Write whatever content we already have to output.
    if(response.size() > 0)
      ostr << &response;

    // Read until EOF, writing data to output as we go.
    while(boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
        ostr << &response;
    if(error != boost::asio::error::eof)
        throw boost::system::system_error(error);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
size_t grib_grabber::read_until(std::istream &istr, const string &srchstr)
{
    const size_t srchlen = srchstr.length();
    size_t moved = 0;

    std::deque<char> buf;
    while(!istr.eof())
    {
        buf.push_back(istr.get());
        ++moved;
        if(buf.size() >= srchlen)
            if(std::equal(buf.end() - srchlen, buf.end(), srchstr.begin()))
                break;
        if(buf.size() > 3 * srchlen)
        {
            std::deque<char>::iterator eit = buf.begin();
            std::advance(eit, srchlen * 2);
            buf.erase(buf.begin(), eit);
        }
    }
    return moved;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber::dispatch_grib_data(std::istream &istr, list<request> &requests, const boost::unordered_set<point_ll_deg> &sel_locations)
{
    if(requests.size() == 1)
    {
        read_grib_data(istr, requests.front(), sel_locations);
        return;
    }

    while(!istr.eof() && requests.size())
    {
        const size_t moved = read_until(istr, "Content-range: bytes ");
        if(istr.eof())
            throw std::runtime_error("invalid range response: search string not found: 'Content-range: bytes '");

        string line;
        std::getline(istr, line);

        const size_t posm = line.find('-');
        if(posm == string::npos)
            throw std::runtime_error("invalid range response: '-' not found");
        const size_t poss = line.find('/');
        if(poss == string::npos)
            throw std::runtime_error("invalid range response: '/' not found");
        const size_t rng_start = lexical_cast<size_t>(line.substr(0, posm));
        const size_t rng_end   = lexical_cast<size_t>(line.substr(posm + 1, poss - posm - 1));

        const size_t skipped = read_until(istr, "\r\n");
//        cout << "skipped " << skipped << " bytes until the next \\r\\n" << endl;

        // find the request that matches the byte position of the response
        list<request>::iterator fit = std::find_if(requests.begin(), requests.end(),
                                                     rng_start == bind(&request::range_start, _1));
        if(fit == requests.end())
        {
            cout << "requested : " << endl;
            std::transform(requests.begin(), requests.end(), std::ostream_iterator<size_t>(cout, "/"), bind(&request::range_start, _1));
            cout << endl << "found range: " << rng_start << " - " << rng_end << endl;

            throw std::runtime_error("returned range was not requested.");
        }

        read_grib_data(istr, *fit, sel_locations);

        requests.erase(fit);
    }

    if(requests.size())
    {
        cout << requests.size() << " requests remaining!" << endl;
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber::read_grib_data(std::istream &istr, const request &req, const boost::unordered_set<point_ll_deg> &sel_locations)
{
    const size_t len = (req.range_end > req.range_start ?
                        req.range_end - req.range_start :
                        std::numeric_limits<size_t>::max());
    vector<char> buf;
    for(size_t i=0; i<len && !istr.eof(); ++i)
        buf.push_back(istr.get());

    cout << req.pred_time << " " << req.level << " " << req.param << " ";

    grib_handle *h = grib_handle_new_from_message(0, &buf[0], buf.size());

    int err = 0;
    // Get the double representing the missing value in the field.
    double missingValue = 0.0;
    err = grib_get_double(h, "missingValue", &missingValue);
    if(err)
        throw std::runtime_error("failed to get grib missing value - error code: " + lexical_cast<string>(err));

    // A new iterator on lat/lon/values is created from the message handle h.
    grib_iterator *iter = grib_iterator_new(h, 0, &err);
    if(err != GRIB_SUCCESS)
        throw std::runtime_error("failed to create grib iterator - error code: " + lexical_cast<string>(err));

    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "insert grib data");

    int count = 0;
    double lat, lon, value;
    // Loop on all the lat/lon/values.
    while(grib_iterator_next(iter, &lat, &lon, &value))
        if(value != missingValue)
        {
//            cout << bgreg::to_iso_extended_string(req.pred_time.date()) << " " << lon << " " << lat << endl;

//            if(lat < 40.0 || lat > 55.0 || lon < -5.0 || lon > 20.0)
//                continue; // for the moment we collect only data from central europe

            const point_ll_deg location = point_ll_deg(geometry::longitude<>(lon), geometry::latitude<>(lat));
            if(sel_locations.find(location) == sel_locations.end())
                continue;

            std::stringstream sstr;
            sstr << bgreg::to_iso_extended_string(req.pred_time.date()) << " " << std::setfill('0')
                 << std::setw(2) << req.pred_time.time_of_day().hours() << ":00:00";
            const string dati(sstr.str());

            sstr.str("");
            sstr << "SELECT weather_pred_id FROM weather_pred "
                 << "WHERE location = GeomFromText('POINT(" << lon << " " << lat << ")', " << PG_SIR_WGS84 << ") "
                 << "AND pred_time='" << dati << "' "
                 << "AND level=" << req.level
                 << "AND parameter='" << req.param << "'";
            pqxx::result res = trans.exec(sstr.str());
            if(res.size()) // the record is already in the database
            {
                if(req.pred_time < bpt::second_clock::universal_time() - bpt::hours(2))
                    continue;
                sstr.str("");
                sstr << "DELETE FROM weather_pred "
                     << "WHERE location = GeomFromText('POINT(" << lon << " " << lat << ")', " << PG_SIR_WGS84 << ") "
                     << "AND pred_time='" << dati << "' "
                     << "AND level=" << req.level
                     << "AND parameter='" << req.param << "'";
                res = trans.exec(sstr.str());
            }

            sstr.str("");
            sstr << "INSERT INTO weather_pred (pred_time, level, parameter, location, value) "
                 << "values ('" << dati<< "', " << req.level << ", '" << req.param
                 << "', GeomFromText('POINT(" << lon << " " << lat << ")', " << PG_SIR_WGS84 << "), "
                 << value << ")";
            res = trans.exec(sstr.str());
            if(++count % 10 == 0)
            {
                cout << "|";
                cout.flush();
            }
        }
        else
            cout << " missing value " << std::endl;

    trans.commit();

    grib_iterator_delete(iter);
    grib_handle_delete(h);

    cout << endl << "imported " << count << " records." << endl;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
set<string> grib_grabber::get_std_levels()
{
    set<string> sel_levels;

    sel_levels.insert("sfc");
    sel_levels.insert("surface");
    sel_levels.insert("850 mb");
    sel_levels.insert("700 mb");
    sel_levels.insert("500 mb");

    return sel_levels;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
set<string> grib_grabber::get_std_params()
{
    set<string> sel_params;

    sel_params.insert("PRES");
    sel_params.insert("TMP");
    sel_params.insert("HGT");
    sel_params.insert("UGRD");
    sel_params.insert("VGRD");
    sel_params.insert("ABSV");
    sel_params.insert("RH");
    sel_params.insert("VVEL");
    sel_params.insert("CLWMR");

    return sel_params;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
struct pnt_ll_deg_dist_sorter
{
    pnt_ll_deg_dist_sorter(const point_ll_deg &srch_pnt)
        : srch_pnt_(srch_pnt), strategy_(geometry::strategy::distance::vincenty<point_ll_deg>()) { }

    bool operator()(const point_ll_deg &lhs, const point_ll_deg &rhs)
    {
        const double dist1 = geometry::distance(lhs, srch_pnt_, strategy_);
        const double dist2 = geometry::distance(rhs, srch_pnt_, strategy_);
        return dist1 < dist2;
    }

    const point_ll_deg srch_pnt_;
    const geometry::strategy::distance::vincenty<geometry::point_ll_deg> strategy_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
boost::unordered_set<point_ll_deg> grib_grabber::get_locations_around_sites(const double gridres, const size_t pnts_per_site) const
{
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "get training locations");

    pqxx::result res = trans.exec("SELECT AsText(location) AS loc FROM pred_sites");
    if(!res.size())
        throw std::runtime_error("no prediction sites found");

    boost::unordered_set<point_ll_deg> locations;
    for(size_t i=0; i<res.size(); ++i)
    {
        string locstr;
        res[0]["loc"].to(locstr);
        point_ll_deg site_location;
        if(!geometry::from_wkt(locstr, site_location))
            throw std::runtime_error("invalid location read from pred_sites : " + locstr);

        vector<point_ll_deg> tmploc;
        double lonl = static_cast<int>(site_location.lon() / gridres) * gridres;
        double latl = static_cast<int>(site_location.lat() / gridres) * gridres;
        for(int i=pnts_per_site / -2; i < pnts_per_site / 2; ++i)
            for(int j=pnts_per_site / -2; j < pnts_per_site / 2; ++j)
                tmploc.push_back(point_ll_deg(geometry::longitude<>(lonl + i * gridres),
                                              geometry::latitude<>( latl + j * gridres)));
        assert(tmploc.size() > pnts_per_site);
        std::sort(tmploc.begin(), tmploc.end(), pnt_ll_deg_dist_sorter(site_location));
        vector<point_ll_deg>::iterator endsel = tmploc.begin();
        std::advance(endsel, pnts_per_site);
        std::copy(tmploc.begin(), endsel, std::inserter(locations, locations.end()));
    }

    return locations;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber_gfs_past::grab_grib(const bgreg::date &from, const bgreg::date &to)
{
    const set<string>       sel_levels    = get_std_levels();
    const set<string>       sel_param     = get_std_params();
    const boost::unordered_set<point_ll_deg> sel_locations = get_locations_around_sites(2.5, 20);

    // download the grib files
    for(bgreg::date mon(from.year(), from.month(), 1); mon <= to; mon += bgreg::months(1))
    {
        // first, get the inventory
        std::stringstream ssurl;
        ssurl << baseurl_ << "pgb." << std::setfill('0') << std::setw(4) << static_cast<int>(mon.year())
              << std::setw(2) << static_cast<int>(mon.month());

        std::stringstream buf_inv;
        download_data(ssurl.str() + ".inv", buf_inv, list<request>(), false);

        list<request> requests;
        string line;
        while(std::getline(buf_inv, line))
        {
            boost::char_separator<char> sep(":");
            boost::tokenizer<boost::char_separator<char> > tokens(line, sep);
            vector<string> vtokens;
            std::copy(tokens.begin(), tokens.end(), back_inserter(vtokens));
            if(vtokens.size() < 7)
                continue;
            // todo : grib2 has a range field
            request req;
            req.range_start = lexical_cast<size_t>(vtokens[1]);
            req.range_end   = 0;
            if(requests.size() && !requests.back().range_end)
                requests.back().range_end = req.range_start;
            if(vtokens[2].length() == 12 && vtokens[2].substr(0, 2) == "d=")
            {
                req.pred_time = bpt::ptime(bgreg::from_undelimited_string(vtokens[2].substr(2, 8)),
                                           bpt::hours(lexical_cast<int>(vtokens[2].substr(10, 2))));
                if(req.pred_time.date() >=from && req.pred_time.date() <= to)
                    if(sel_levels.find(vtokens[4]) != sel_levels.end() && sel_param.find(vtokens[3]) != sel_param.end())
                    {
                        req.level = atoi(vtokens[4].c_str());
                        req.param = vtokens[3];

                        if(requests.size() >= download_pack_)
                        {
                            // download the data
                            stringstream buf_grib(ios_base::out | ios_base::in | ios_base::binary);
                            download_data(ssurl.str(), buf_grib, requests, false);

                            // decode the grib data and write into the database
                            dispatch_grib_data(buf_grib, requests, sel_locations);

                            requests.clear();
                        }

                        requests.push_back(req);
                    }
            }
        } // while getline

        if(requests.size())
        {
            // download the data
            stringstream buf_grib(ios_base::out | ios_base::in | ios_base::binary);
            download_data(ssurl.str(), buf_grib, requests, true);

            // decode the grib data and write into the database
            dispatch_grib_data(buf_grib, requests, sel_locations);
        }
    } // for day
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber_gfs_future::grab_grib()
{
    const set<string>       sel_levels    = get_std_levels();
    const set<string>       sel_param     = get_std_params();
    const boost::unordered_set<point_ll_deg> sel_locations = get_locations_around_sites(2.5, 20);

    const bpt::time_duration usual_pred_run_time = bpt::hours(18); // the usual mxa time for the prediction results to become online available
    const bpt::ptime now = bpt::second_clock::universal_time() - usual_pred_run_time;
    const size_t   hours = static_cast<size_t>(now.time_of_day().hours() / 6.0) * 6.0;
    const bpt::ptime lastpredrun(now.date(), bpt::hours(hours));
    const bpt::ptime lastmidnight(now.date(), bpt::hours(0));
    const bpt::ptime yesterday(now.date() - bgreg::days(1), bpt::hours(0));

    // download the grib files
    for(bpt::ptime preddt = lastmidnight; preddt < now + bpt::hours(72); preddt += bpt::hours(6))
    {
        bpt::ptime predrun     = lastpredrun;

        if(preddt >= lastpredrun)
            predrun = lastpredrun;
        else if(preddt >= lastmidnight)
            predrun = lastmidnight;
        else if(preddt >= yesterday)
            predrun = yesterday;
        else
            assert(!"how come?");

        const size_t futurehours = (preddt - predrun).hours();


        // first, get the inventory
        std::stringstream ssurl;
        ssurl << baseurl_ << "gfs" << std::setfill('0')
              << std::setw(4) << static_cast<int>(predrun.date().year())
              << std::setw(2) << static_cast<int>(predrun.date().month())
              << std::setw(2) << static_cast<int>(predrun.date().day())
              << "/gfs.t"   << std::setw(2) << static_cast<int>(predrun.time_of_day().hours())
              << "z.master.grbf" << std::setw(2) << futurehours;

        std::stringstream buf_inv;
        download_data(ssurl.str() + ".inv", buf_inv, list<request>(), false);

        list<request> requests;
        string line;
        while(std::getline(buf_inv, line))
        {
            boost::char_separator<char> sep(":");
            boost::tokenizer<boost::char_separator<char> > tokens(line, sep);
            vector<string> vtokens;
            std::copy(tokens.begin(), tokens.end(), back_inserter(vtokens));
            if(vtokens.size() < 6)
                continue;
            request req;
            req.range_start = lexical_cast<size_t>(vtokens[1]);
            req.range_end   = 0;
            if(requests.size() && !requests.back().range_end)
                requests.back().range_end = req.range_start;
            if(vtokens[2].length() == 12 && vtokens[2].substr(0, 2) == "d=")
            {
//                req.pred_time = bpt::ptime(bgreg::from_undelimited_string(vtokens[2].substr(2, 8)),
//                                           bpt::hours(lexical_cast<int>(vtokens[2].substr(10, 2))));
                req.pred_time = preddt;
//                assert(req.pred_time == preddt);
                if(sel_levels.find(vtokens[4]) != sel_levels.end() && sel_param.find(vtokens[3]) != sel_param.end())
                {
                    req.level = atoi(vtokens[4].c_str());
                    req.param = vtokens[3];

                    if(requests.size() >= download_pack_)
                    {
                        list<request>::iterator sepi = std::remove_if(requests.begin(), requests.end(),
                            bind(&request::range_end, _1) == bind(&request::range_start, _1));
                        for(list<request>::const_iterator it = sepi; it != requests.end(); ++it)
                            cout << " zero length for " << it->level << " " << it->param << endl;
                        requests.erase(sepi, requests.end());

                        // download the data
                        stringstream buf_grib(ios_base::out | ios_base::in | ios_base::binary);
                        download_data(ssurl.str(), buf_grib, requests, false);

                        // decode the grib data and write into the database
                        dispatch_grib_data(buf_grib, requests, sel_locations);

                        requests.clear();
                    }

                    requests.push_back(req);
                }
            }
        } // while getline

        for(int i=0; i<10 && requests.size(); ++i) // looks like we have to look a couple of times to get everything
        {
            if(i)
                cout << "retry " << i << endl;
            // download the data
            stringstream buf_grib(ios_base::out | ios_base::in | ios_base::binary);
            download_data(ssurl.str(), buf_grib, requests, true);

            // decode the grib data and write into the database
            dispatch_grib_data(buf_grib, requests, sel_locations);
        }

        if(requests.size())
        {
            BOOST_FOREACH(request &req, requests)
                cout << req.param << " " << req.level << " " << req.range_start << " " << req.range_end << endl;
        }
    } // for day
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
size_t geometry::hash_value(const geometry::point_ll_deg &pnt)
{
    return boost::hash_value(pnt.lat()) + boost::hash_value(pnt.lon());
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
bool geometry::operator==(const geometry::point_ll_deg &lhs, const geometry::point_ll_deg &rhs)
{
    return lhs.lon() == rhs.lon() && lhs.lat() == rhs.lat();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A


