#ifndef MAINFLIGHTPREDWT_H
#define MAINFLIGHTPREDWT_H

// flightpred

// witty
#include <Wt/WApplication>
// boost
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
//standard library
#include <string>
#include <vector>
#include <utility>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace Wt
{
    class WContainerWidget;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class FlightpredApp : public Wt::WApplication
{
public:
    FlightpredApp(const Wt::WEnvironment& env);

protected:
    virtual void finalize();
    void makePredDay(const boost::gregorian::date &day, Wt::WContainerWidget *parentForTable, Wt::WContainerWidget *parentForMap);
    void showWeatherData(Wt::WContainerWidget *parent, const std::string &model_name, const std::string &param, size_t level, const boost::posix_time::ptime &when);
private:
    void try_imbue(std::ostream &ostr, const std::string &localename);


    std::string db_conn_str_;
    std::vector<std::pair<size_t, std::string> > sites_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // MAINFLIGHTPREDWT_H
