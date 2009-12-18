// flightpred
#include "common/solution_manager.h"
#include "common/features_weather.h"
#include "common/flightpred_globals.h"
#include "common/ga_evocosm.h"
#include "common/reporter.h"
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
// boost
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
// std lib
#include <iostream>

using namespace flightpred;
using namespace flightpred::reporting;
using geometry::point_ll_deg;
using boost::lexical_cast;
using boost::shared_ptr;
namespace bgreg = boost::gregorian;
using std::string;
using std::vector;
using std::map;
using std::set;



/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void solution_manager::fill_label_cache()
{
    report(VERBOSE) << "collecting lables";
    pqxx::transaction<> trans(flightpred_db::get_conn(), "fill_max_distance_cache");
    std::stringstream sstr;
    sstr << "SELECT flight_date, MAX(distance) FROM flights INNER JOIN sites "
         << "ON flights.site_id = sites.site_id "
         << "WHERE pred_site_id = " << pred_site_id_ << " "
         << "GROUP BY flight_date "
         << "ORDER BY flight_date ASC";
    pqxx::result res = trans.exec(sstr.str());
    for(size_t i=0; i<res.size(); ++i)
    {
        string datestr;
        res[i][0].to(datestr);
        bgreg::date day(bgreg::from_string(datestr));
        double max_dist;
        res[i][1].to(max_dist);
        if(max_dist < 1.0)
            max_dist = 0.0;
        max_distances_[day] = max_dist;
    }

    training_labels_.clear();
    for(bgreg::day_iterator dit(date_extremes_.begin()); *dit <= date_extremes_.end(); ++dit)
        if(used_for_training(*dit))
            training_labels_.push_back(max_distances_[*dit]);

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const vector<double> & solution_manager::get_sample(const set<features_weather::feat_desc> &fdesc, const bgreg::date &day)
{
    WeatherFeatureCacheType::const_iterator fitFdesc = weathercache_.find(fdesc);
    if(fitFdesc != weathercache_.end())
    {
        map<bgreg::date, vector<double> >::const_iterator fitDt = fitFdesc->second.find(day);
        if(fitDt != fitFdesc->second.end())
            return fitDt->second;
    }

    map<bgreg::date, vector<double> > &featsol = weathercache_[fdesc];

    report(VERBOSE) << "collecting features for " << bgreg::to_iso_extended_string(day);

    const vector<double> valweather = weather_.get_features(fdesc, day, false);
    assert(valweather.size() == fdesc.size());

    vector<double> &samples = featsol[day];
    // put together the values to feed to the svm
    samples.push_back(day.year());
    samples.push_back(day.day_of_year());
    samples.push_back(day.day_of_week());
    std::copy(valweather.begin(), valweather.end(), std::back_inserter(samples));

    return samples;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief get an initial generation of solutions. They vary in algorithms and their parameters, but not yet by feature sets */
vector<shared_ptr<solution_config> > solution_manager::get_initial_generation()
{
    pqxx::transaction<> trans(flightpred_db::get_conn(), "initial generation");

    // get the geographic position of the prediction site
    std::stringstream sstr;
    sstr << "SELECT AsText(location) as loc FROM pred_sites WHERE site_name='" << site_name_ << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::invalid_argument("site not found : " + site_name_);
    string tmpstr;
    res[0]["loc"].to(tmpstr);
    geometry::point_ll_deg pred_location;
    if(!geometry::from_wkt(tmpstr, pred_location))
        throw std::runtime_error("failed to parse the prediction site location as retured from the database : " + tmpstr);

    // get the default feature descriptions of the weather data
    const set<features_weather::feat_desc> features = weather_.get_standard_features(pred_location);
    sstr.str("");
    std::copy(features.begin(), features.end(), std::ostream_iterator<features_weather::feat_desc>(sstr, " "));
    const string weather_feat_desc(sstr.str());

    vector<string> algo_desc;
    algo_desc.push_back("DLIB_RVM(RBF(0.1))");
    algo_desc.push_back("DLIB_RVM(RBF(0.05))");
    algo_desc.push_back("DLIB_RVM(RBF(0.02))");
    algo_desc.push_back("DLIB_RVM(RBF(0.01))");
    algo_desc.push_back("DLIB_RVM(RBF(0.007))");
    algo_desc.push_back("DLIB_RVM(RBF(0.005))");
    algo_desc.push_back("DLIB_RVM(RBF(0.001))");
    algo_desc.push_back("DLIB_RVM(RBF(0.0001))");
    algo_desc.push_back("DLIB_KRLS(RBF(0.5)    0.5)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.1)    0.5)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01)   0.5)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01)   0.1)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01)   0.01)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01)   0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01)   0.0001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01)   0.00001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.007)  0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.005)  0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.001)  0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.0001) 0.001)");
    algo_desc.push_back("DLIB_RVM(SIG(0.1   0.1))");
    algo_desc.push_back("DLIB_RVM(SIG(0.1   0.01))");
    algo_desc.push_back("DLIB_RVM(SIG(0.1   0.001))");
    algo_desc.push_back("DLIB_RVM(SIG(0.01  0.1))");
    algo_desc.push_back("DLIB_RVM(SIG(0.01  0.01))");
    algo_desc.push_back("DLIB_RVM(SIG(0.01  0.001))");
    algo_desc.push_back("DLIB_RVM(SIG(0.001 0.1))");
    algo_desc.push_back("DLIB_RVM(SIG(0.001 0.01))");
    algo_desc.push_back("DLIB_RVM(SIG(0.001 0.001))");
    algo_desc.push_back("DLIB_RVM(POLY(0.1   0.1   0.1))");
    algo_desc.push_back("DLIB_RVM(POLY(0.1   0.1   0.01))");
    algo_desc.push_back("DLIB_RVM(POLY(0.1   0.1   0.001))");
    algo_desc.push_back("DLIB_RVM(POLY(0.1   0.01  0.1))");
    algo_desc.push_back("DLIB_RVM(POLY(0.1   0.01  0.01))");
    algo_desc.push_back("DLIB_RVM(POLY(0.1   0.01  0.001))");
    algo_desc.push_back("DLIB_RVM(POLY(0.1   0.001 0.1))");
    algo_desc.push_back("DLIB_RVM(POLY(0.1   0.001 0.01))");
    algo_desc.push_back("DLIB_RVM(POLY(0.1   0.001 0.001))");
    algo_desc.push_back("DLIB_RVM(POLY(0.01  0.1   0.1))");
    algo_desc.push_back("DLIB_RVM(POLY(0.01  0.1   0.01))");
    algo_desc.push_back("DLIB_RVM(POLY(0.01  0.1   0.001))");
    algo_desc.push_back("DLIB_RVM(POLY(0.01  0.01  0.1))");
    algo_desc.push_back("DLIB_RVM(POLY(0.01  0.01  0.01))");
    algo_desc.push_back("DLIB_RVM(POLY(0.01  0.01  0.001))");
    algo_desc.push_back("DLIB_RVM(POLY(0.01  0.001 0.1))");
    algo_desc.push_back("DLIB_RVM(POLY(0.01  0.001 0.01))");
    algo_desc.push_back("DLIB_RVM(POLY(0.01  0.001 0.001))");
    algo_desc.push_back("DLIB_RVM(POLY(0.001 0.1   0.1))");
    algo_desc.push_back("DLIB_RVM(POLY(0.001 0.1   0.01))");
    algo_desc.push_back("DLIB_RVM(POLY(0.001 0.1   0.001))");
    algo_desc.push_back("DLIB_RVM(POLY(0.001 0.01  0.1))");
    algo_desc.push_back("DLIB_RVM(POLY(0.001 0.01  0.01))");
    algo_desc.push_back("DLIB_RVM(POLY(0.001 0.001  0.001))");
    algo_desc.push_back("DLIB_RVM(POLY(0.001 0.001 0.1))");
    algo_desc.push_back("DLIB_RVM(POLY(0.001 0.001 0.01))");
    algo_desc.push_back("DLIB_RVM(POLY(0.001 0.001 0.001))");


    vector<shared_ptr<solution_config> > solutions;
    BOOST_FOREACH(const string &desc, algo_desc)
        solutions.push_back(shared_ptr<solution_config>(new solution_config(site_name_, desc + " " + weather_feat_desc, 0)));

    return solutions;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// todo : this will have to get a real implementation. What is here at the moment is only to get it to compile with libevocosm.
evolution::organism create_ramdom_solution(const string &site_name)
{
    assert(!"should not get here as we use a non-random initial generation");
    size_t db_id = 0;
    return evolution::organism(solution_config(db_id));
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void solution_manager::evolve_population(const size_t iterations, const double mutation_rate)
{
    fill_label_cache();

    const size_t population_size = get_initial_generation().size();
    flightpred::evolution::optimizer opt(boost::bind(&solution_manager::test_fitness, this, ::_1),
                                         boost::bind(&create_ramdom_solution, site_name_),
                                         boost::bind(&solution_manager::initialize_population, this),
                                         population_size,
                                         mutation_rate,
                                         iterations,
                                         pred_site_id_);
    opt.run();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief initialize a population to start the evolution. If we have sufficient organisms in the db, use them. Otherwise generate an initial generation */
vector<shared_ptr<solution_config> > solution_manager::initialize_population()
{
    vector<shared_ptr<solution_config> > population = get_initial_generation();

    // look in the database
    pqxx::transaction<> trans(flightpred_db::get_conn(), "solution_manager::initialize_population");
    std::stringstream sstr;
    sstr << "SELECT COUNT(train_sol_id) FROM trained_solutions "
         << "WHERE pred_site_id=" << pred_site_id_;
    pqxx::result res = trans.exec(sstr.str());
    size_t numrecords;
    res[0][0].to(numrecords);
    if(numrecords > population.size())
    {
        // we have enough solutions in the database from previous runs, that we can continue there
        sstr.str("");
        sstr << "SELECT train_sol_id FROM trained_solutions "
             << "WHERE pred_site_id=" << pred_site_id_ << " "
             << "ORDER BY generation DESC, validation_error ASC "
             << "LIMIT " << population.size();
        res = trans.exec(sstr.str());
        vector<size_t> ids;
        for(size_t i=0; i<res.size(); ++i)
        {
            size_t id;
            res[i][0].to(id);
            ids.push_back(id);
        }
        trans.commit();

        population.clear();
        BOOST_FOREACH(size_t id, ids)
            population.push_back(shared_ptr<solution_config>(new solution_config(id)));
    }

    return population;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief test how well a solution can predict the flights.
    @return the total error in km */
const double solution_manager::test_fitness(const solution_config &sol)
{
    report(VERBOSE) << "collect features for : " << sol.get_short_description() <<  std::endl;
    learning_machine::SampleType samples;

    for(bgreg::day_iterator dit(date_extremes_.begin()); *dit <= date_extremes_.end(); ++dit)
        if(used_for_training(*dit))
            samples.push_back(get_sample(sol.get_weather_feature_desc(), *dit));
    boost::timer btim;
    static const string eval_name("max_dist");
    report(VERBOSE) << "train the learning machine : " << sol.get_short_description() <<  std::endl;
    sol.get_decision_function(eval_name)->train(samples, training_labels_);
    const double traintime = btim.elapsed();
    report(VERBOSE) << "training took " << btim.elapsed() << " sec" << std::endl;

    // validate
    report(VERBOSE) << "validate the learning machine : " << sol.get_short_description() <<  std::endl;
    double sum_err = 0;
    for(bgreg::day_iterator dit(date_extremes_.begin()); *dit <= date_extremes_.end(); ++dit)
    {
        if(used_for_validation(*dit))
        {
            const vector<double> &samples = get_sample(sol.get_weather_feature_desc(), *dit);
            const double predval   = sol.get_decision_function(eval_name)->eval(samples);
            const double predval_p = predval > 0.0 ? predval : 0.0; // to catch nan
            const double & realval = max_distances_[*dit];
            const double err = fabs(realval - predval_p);
            sum_err  += err;
        }
    }
    report(INFO) << "validation result for : " << sol.get_short_description()
                 << " gen " << sol.get_generation()
                 << " total error in km is " << sum_err <<  std::endl;

    // todo : handle different generations
    pqxx::transaction<> trans(flightpred_db::get_conn(), "solution_manager::test_fitness");
    std::stringstream sstr;
    sstr << "INSERT INTO trained_solutions (generation, pred_site_id, configuration, validation_error, train_time, num_features) VALUES "
         << "(" << sol.get_generation() << ", " << pred_site_id_ << ", '" << sol.get_description() << "', " << sum_err << ", "
         << traintime << ", " << sol.get_weather_feature_desc().size() << ")";
    trans.exec(sstr.str());
    trans.commit();

    return sum_err;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const bool solution_manager::used_for_training(const bgreg::date &day) const
{
    return day.day_of_week() == 0; // sunday
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const bool solution_manager::used_for_validation(const bgreg::date &day) const
{
    return day.day_of_week() == 6; // saturday
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
size_t solution_manager::get_pred_site_id(const std::string &site_name)
{
    pqxx::transaction<> trans(flightpred_db::get_conn(), "solution_manager::get_pred_site_id");

    std::stringstream sstr;
    sstr << "SELECT pred_site_id FROM pred_sites where site_name='" << site_name << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::invalid_argument("site not found: " + site_name);

    size_t pred_site_id;
    res[0]["pred_site_id"].to(pred_site_id);
    return pred_site_id;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
std::auto_ptr<solution_config> solution_manager::load_best_solution(const bool onlyFullyTrained, const double maxTrainSec)
{
    pqxx::transaction<> trans(flightpred_db::get_conn(), "load_best_solution");

    std::stringstream sstr;
    sstr << "SELECT train_sol_id, configuration, score, train_time, generation FROM trained_solutions WHERE ";
    if(onlyFullyTrained)
        sstr << "num_samples_prod > 300 AND ";
    if(maxTrainSec > 0.0)
        sstr << "train_time <= " << maxTrainSec << " AND ";
    sstr << "pred_site_id=" << pred_site_id_ << " ORDER BY validation_error ASC, generation DESC";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("no configuration found for : " + site_name_);

    size_t solution_id;
    res[0]["train_sol_id"].to(solution_id);
    trans.commit();

    return std::auto_ptr<solution_config>(new solution_config(solution_id));
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
