// flightpred
#include "grab_grib.h"
#include "geo_parser.h"
// grib api
#include "grib_api.h"
// postgre
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/geometry.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/asio.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
// standard library
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <limits>
//#include <cstring>



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
grib_grabber::grib_grabber(const std::string &db_conn_str, const std::string &model, size_t download_pack)
    : db_conn_str_(db_conn_str), model_(get_grib_pred_model(model)), download_pack_(download_pack)
{

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber::grab_grib(const bgreg::date &from, const bgreg::date &to)
{
    set<string> sel_levels;
    sel_levels.insert("sfc");
    sel_levels.insert("850 mb");
    sel_levels.insert("700 mb");
    sel_levels.insert("500 mb");

    // download the grib files
    const string baseurl = "nomad3.ncep.noaa.gov/pub/reanalysis-2/6hr/pgb/pgb.";

    for(bgreg::date mon(from.year(), from.month(), 1); mon <= to; mon += bgreg::months(1))
    {
        // first, get the inventory
        std::stringstream ssurl;
        ssurl << baseurl << std::setfill('0') << std::setw(4) << static_cast<int>(mon.year())
              << std::setw(2) << static_cast<int>(mon.month());

        std::stringstream buf_inv;
        download_data(ssurl.str() + ".inv", buf_inv, list<request>());

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
                    if(sel_levels.find(vtokens[4]) != sel_levels.end())
                    {
                        req.level = atoi(vtokens[4].c_str());
                        req.param = vtokens[3];

                        if(requests.size() >= download_pack_)
                        {
                            // download the data
                            stringstream buf_grib(ios_base::out | ios_base::in | ios_base::binary);
                            download_data(ssurl.str(), buf_grib, requests);

                            // decode the grib data and write into the database
                            dispatch_grib_data(buf_grib, requests);

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
            download_data(ssurl.str(), buf_grib, requests);

            // decode the grib data and write into the database
            dispatch_grib_data(buf_grib, requests);
        }
    } // for day
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber::download_data(const string &url, std::ostream &ostr, const list<request> &requests)
{
    std::cout << "downloading : " << url << std::endl;

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
    if(requests.size())
    {
        request_stream << "Range: bytes=";
        for(list<grib_grabber::request>::const_iterator it = requests.begin(); it != requests.end(); ++it)
        {
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
    std::string header;
    while(std::getline(response_stream, header) && header != "\r")
        std::cout << header << "\n";
    std::cout << "\n";

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
void grib_grabber::dispatch_grib_data(std::istream &istr, list<request> &requests)
{
    if(requests.size() == 1)
    {
        read_grib_data(istr, requests.front());
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

        read_until(istr, "\r\n");

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

        read_grib_data(istr, *fit);

        requests.erase(fit);
    }

    if(requests.size())
    {
        cout << requests.size() << " requests remaining!" << endl;
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber::read_grib_data(std::istream &istr, const request &req)
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
            if(lat < 40.0 || lat > 55.0 || lon < -5.0 || lon > 20.0)
                continue; // for the moment we collect only data from central europe

            std::stringstream sstr;
            sstr << bgreg::to_iso_extended_string(req.pred_time.date()) << " " << std::setfill('0')
                 << std::setw(2) << req.pred_time.time_of_day().hours() << ":00:00";
            const string dati(sstr.str());

            sstr.str("");
            sstr << "SELECT weather_pred_id FROM weather_pred "
                 << "WHERE location = GeomFromText('POINT(" << lat << " " << lon << ")', -1) "
                 << "AND pred_time='" << dati << "' "
                 << "AND level=" << req.level
                 << "AND parameter='" << req.param << "'";
            pqxx::result res = trans.exec(sstr.str());
            if(res.size())
                continue; // the record is already in the database

            sstr.str("");
            sstr << "INSERT INTO weather_pred (pred_time, level, parameter, location, value) "
                 << "values ('" << dati<< "', " << req.level << ", '" << req.param
                 << "', GeomFromText('POINT(" << lat << " " << lon << ")', -1), "
                 << value << ")";
            res = trans.exec(sstr.str());
            if(++count % 10 == 0)
            {
                cout << "|";
                cout.flush();
            }
        }

    trans.commit();

    grib_iterator_delete(iter);
    grib_handle_delete(h);

    cout << endl << "imported " << count << " records." << endl;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
