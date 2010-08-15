#ifndef FORECAST_H_INCLUDED
#define FORECAST_H_INCLUDED

// flightpred

#include "learning_machine.h"
// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/latlong/latlong.hpp>
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
    forecast() { }
    ~forecast() { };

    void  prediction_run(const std::string &site_name, const size_t pred_days);
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

#endif // FORECAST_H_INCLUDED
