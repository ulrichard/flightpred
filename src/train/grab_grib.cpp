// flightpred
#include "grab_grib.h"
#include "geo_parser.h"
// postgre
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <geometry/geometry.hpp>
//#include <geometry/io/wkt/streamwkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/asio.hpp>
// standard library
#include <sstream>
#include <iomanip>


using namespace flightpred;
using geometry::point_ll_deg;
namespace bfs = boost::filesystem;
namespace bgreg = boost::gregorian;
namespace bpt = boost::posix_time;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using boost::lexical_cast;
using boost::asio::ip::tcp;
using std::string;
using std::vector;
using std::map;
using std::set;

// a description of the grib data fields can be found at:
// http://www.nco.ncep.noaa.gov/pmb/products/gfs/gfs.t00z.pgrb2f00.shtml
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
grib_grabber::grib_grabber(const std::string &db_conn_str, const std::string &model)
    : db_conn_str_(db_conn_str), model_(get_grib_pred_model(model))
{

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber::grab_grib(const bgreg::date &from, const bgreg::date &to, const point_ll_deg &pos)
{
    // validate that the requested location falls within the grid of the prediction model
    int lat100  = pos.lat() * 100.0;
    int lon100  = pos.lon() * 100.0;
    int grid100 = model_->getGridResolution() * 100;
    if(lat100 % grid100)
        throw std::invalid_argument("Requested position doesn't match the prediction model's grid.");
    if(lon100 % grid100)
        throw std::invalid_argument("Requested position doesn't match the prediction model's grid.");
    // check if we already have the data
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "import grib data");
    // contest
    std::stringstream sstr;
    sstr << "SELECT weather_pred_id FROM weather_pred "
         << "WHERE location = GeomFromText('POINT(" << pos.lat() << " " << pos.lon() << ")', -1) "
         << "AND pred_run='" << bgreg::to_iso_extended_string(from) << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(res.size())
    {
        std::cout << "The requested data is already present in the database. No need to download." << std::endl;
        return;
    }
    // download the grib files
    const string baseurl = "nomad3.ncep.noaa.gov/pub/reanalysis-2/6hr/pgb/pgb.";

    set<string> sel_levels;
    sel_levels.insert("surface");
    sel_levels.insert("850 mb");
    sel_levels.insert("750 mb");
    sel_levels.insert("500 mb");


    for(bgreg::date mon(from.year(), from.month(), 1); mon <= to; mon += bgreg::months(1))
    {
        // first, get the inventory
        std::stringstream sstr;
        sstr << baseurl << std::setfill('0') << std::setw(4) << static_cast<int>(mon.year())
             << std::setw(2) << static_cast<int>(mon.month()) << ".inv";


        std::stringstream buf;
        download_file(sstr.str(), buf);

        size_t lastpos = 0;
        string line;
        while(std::getline(buf, line))
        {
            boost::char_separator<char> sep(":");
            boost::tokenizer<boost::char_separator<char> > tokens(line, sep);
            vector<string> vtokens;
            std::copy(tokens.begin(), tokens.end(), back_inserter(vtokens));
            if(vtokens.size() < 7)
                continue;
            if(sel_levels.find(vtokens[4]) == sel_levels.end())
                continue;
            // todo : grib2 has a range field
            const size_t pos = lexical_cast<size_t>(vtokens[1]);

            // download the data
            std::cout << "download " << lastpos << " to " << pos << std::endl;

            //system("curl -f -v -s -r \"$range\" $url -o $file.tmp");


            lastpos = pos;
        }


    }


    // add the data to the database

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void grib_grabber::download_file(const string &url, std::ostream &ostr)
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
    request_stream << "GET " << path << " HTTP/1.0\r\n";
    request_stream << "Host: " << hostname << "\r\n";
    request_stream << "Accept: */*\r\n";
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
    if(status_code != 200)
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
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
