#ifndef EXTRACT_FEATURES_FLIGHT_H_INCLUDED
#define EXTRACT_FEATURES_FLIGHT_H_INCLUDED


//boost
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/noncopyable.hpp>
// standard library
#include <string>
#include <vector>



namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class feature_extractor_flight : public boost::noncopyable
{
public:
    feature_extractor_flight(const std::string &db_conn_str) : db_conn_str_(db_conn_str) {}
    virtual ~feature_extractor_flight() {}

    std::vector<double> get_features(const std::string &desc, const boost::gregorian::date &day);

private:
    const std::string db_conn_str_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
};

#endif // EXTRACT_FEATURES_FLIGHT_H_INCLUDED
