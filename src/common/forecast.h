#ifndef FORECAST_H_INCLUDED
#define FORECAST_H_INCLUDED

// flightpred
#include "common/features_weather.h"
#include "learning_machine.h"
// ggl (boost sandbox)
#include <geometry/geometries/latlong.hpp>
// boost
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
// standard library
#include <string>
#include <map>
#include <algorithm>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class forecast : public boost::noncopyable
{
public:
    forecast(const std::string &db_conn_str) : db_conn_str_(db_conn_str) { }
    ~forecast() { };

    std::map<std::string, double> predict(const std::string &site_name, const boost::gregorian::date &pred_day);
    void                          prediction_run(const size_t pred_days);

private:
    void load_learned_functins(const std::string &site_name);

    template<class ContainerT>
    void load_features(const boost::gregorian::date &pred_day, std::insert_iterator<ContainerT> insit)
    {
        // get the feature description
//      const std::set<features_weather::feat_desc> features_weather::decode_feature_desc(feature_desc_);
        features_weather weather(db_conn_str_);
        const std::set<features_weather::feat_desc> features = weather.get_standard_features(pred_location_);

        // collect the features
        const std::vector<double> valweather = weather.get_features(features, pred_day);

        *insit++ = pred_day.year();
        *insit++ = pred_day.day_of_year();
        *insit++ = pred_day.day_of_week();
        std::copy(valweather.begin(), valweather.end(), insit);
    }

    const std::string       db_conn_str_;
    std::string             site_name_;
    size_t                  solution_id_;
    std::string             feature_desc_;
    geometry::point_ll_deg  pred_location_;
    boost::ptr_vector<learning_machine> learnedfunctions_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

#endif // FORECAST_H_INCLUDED
