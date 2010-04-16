#ifndef FLIGHTFORECAST_H_INCLUDED
#define FLIGHTFORECAST_H_INCLUDED

// witty
#include <Wt/WContainerWidget>
#include <Wt/WCompositeWidget>
// boost
#include <boost/date_time/gregorian/gregorian_types.hpp>
//#include <boost/date_time/posix_time/posix_time_types.hpp>
// std lib
#include <string>
#include <vector>
#include <utility>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class FlightForecast : public Wt::WCompositeWidget
{
public:
    FlightForecast(const std::string &db_conn_str, const size_t forecast_days = 3, Wt::WContainerWidget *parent = 0);
    virtual ~FlightForecast() { }

private:
    void makePredDay(const boost::gregorian::date &day, Wt::WContainerWidget *parentForTable, Wt::WContainerWidget *parentForMap, const bool showMap);
    void try_imbue(std::ostream &ostr, const std::string &localename);

    Wt::WContainerWidget *impl_;
    const std::string db_conn_str_;
    std::vector<std::pair<size_t, std::string> > sites_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // FLIGHTFORECAST_H_INCLUDED
