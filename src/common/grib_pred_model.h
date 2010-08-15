#ifndef GRIB_PRED_MODEL_H_INCLUDED
#define GRIB_PRED_MODEL_H_INCLUDED

// flightpred

// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/latlong/latlong.hpp>
// boost
#include <boost/noncopyable.hpp>
//#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
// standard library
#include <string>
#include <vector>
#include <map>
#include <set>

namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class grib_pred_model;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class grib_measure
{
public:
    grib_measure(const grib_pred_model* pred_model, const boost::geometry::point_ll_deg& pos, size_t lvl, const std::string& prop, double val,
                 const boost::posix_time::ptime& pred_start, const boost::posix_time::time_duration& pred_future);
    grib_measure(const grib_measure& org) : pred_model_(org.pred_model_), pos_(org.pos_), level_(org.level_),
        prop_(org.prop_), pred_start_(org.pred_start_), pred_future_(org.pred_future_), val_(org.val_) { };

    // getter
    const boost::geometry::point_ll_deg&     pos()             const { return pos_; }
    size_t                                   level()           const { return level_; }
    const std::string&                       property()        const { return prop_; }
    const boost::posix_time::ptime&          predictionStart() const { return pred_start_; }
    const boost::posix_time::time_duration&  predictionAhead() const { return pred_future_; }
    const boost::posix_time::ptime           predictedTime()   const { return pred_start_ + pred_future_; }
    double                                   value()           const { return val_; }
    // setter
    void setValue(double val) { val_ = val; }

private:
    const grib_pred_model*            pred_model_;  // the weather prediction model that generated this data sample.
    boost::geometry::point_ll_deg     pos_;          // geographic position
    size_t                            level_;        // pressure level        (must exist in getPressureLevels())
    std::string                       prop_;         // property abbreviation (must exist in getMeasureNames())
    boost::posix_time::ptime          pred_start_;   // when the prediction run started [GMT]
    boost::posix_time::time_duration  pred_future_;  // the prediction time into the futore
    double                            val_;          // the predicted value
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class grib_pred_model // abstract interface
{
public:
    grib_pred_model() {}

    virtual const std::string                                  name()              const = 0;
    virtual const std::map<std::string, std::string>         & getMeasNames()      const;
    virtual double                                             getGridResolution() const = 0;
    virtual const std::set<size_t>                           & getPressureLevels() const = 0;
    virtual const std::set<boost::posix_time::time_duration> & getPredRunTimes()   const = 0;
    virtual const std::set<boost::posix_time::time_duration> & getFutureTimes()    const = 0;
    virtual std::vector<std::pair<size_t, std::string> >       getStandardProperties() const = 0;


};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class grib_pred_model_gfs : public grib_pred_model
{
public:
    grib_pred_model_gfs() {}

    virtual const std::string                                  name() const { return "GFS"; };
    virtual double                                             getGridResolution() const { return 1.0; }; // deg  // todo: read from db
    virtual const std::set<size_t>                           & getPressureLevels() const;
    virtual const std::set<boost::posix_time::time_duration> & getPredRunTimes() const;
    virtual const std::set<boost::posix_time::time_duration> & getFutureTimes() const;
    virtual std::vector<std::pair<size_t, std::string> >       getStandardProperties() const;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
grib_pred_model * get_grib_pred_model(const std::string &name);
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // GRIB_PRED_MODEL_H_INCLUDED
