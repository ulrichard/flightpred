// flightpred
#include "common/solution_manager.h"
#include "common/features_weather.h"
#include "common/flightpred_globals.h"
#include "common/ga_evocosm.h"
#include "common/lm_svm_dlib.h"
#include "common/reporter.h"
#include "common/GenGeomLibSerialize.h"
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/io/wkt/read_wkt.hpp>
#include <boost/geometry/extensions/gis/io/wkt/write_wkt.hpp>
// boost
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
// std lib
#include <iostream>

using namespace flightpred;
using namespace flightpred::reporting;
using boost::geometry::point_ll_deg;
using boost::lexical_cast;
using boost::shared_ptr;
namespace bgreg = boost::gregorian;
namespace bfs   = boost::filesystem;
using std::string;
using std::vector;
using std::map;
using std::set;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void solution_manager::fill_label_cache()
{
    report(DEBUGING) << "solution_manager::fill_label_cache()";
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
    report(DEBUGING) << "solution_manager::get_initial_generation()";
    pqxx::transaction<> trans(flightpred_db::get_conn(), "initial generation");

    // get the geographic position of the prediction site
    std::stringstream sstr;
    sstr << "SELECT AsText(location) as loc FROM pred_sites WHERE site_name='" << site_name_ << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::invalid_argument("site not found : " + site_name_);
    string tmpstr;
    res[0]["loc"].to(tmpstr);
    point_ll_deg pred_location;
    boost::geometry::read_wkt(tmpstr, pred_location);

    // get the default feature descriptions of the weather data
    const set<features_weather::feat_desc> features = weather_.get_standard_features(pred_location);
    sstr.str("");
    std::copy(features.begin(), features.end(), std::ostream_iterator<features_weather::feat_desc>(sstr, " "));
    const string weather_feat_desc(sstr.str());

    vector<string> algo_desc;
//    algo_desc.push_back("DLIB_RVM(RBF(0.1))");
//    algo_desc.push_back("DLIB_RVM(RBF(0.05))");
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
    report(DEBUGING) << "solution_manager::evolve_population(" << iterations << ", " << mutation_rate << ")";
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
    report(DEBUGING) << "solution_manager::initialize_population()";
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
        report(DEBUGING) << "we have enough solutions in the database from previous runs, that we can continue from there";
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


        vector<evolution::organism> organisms;
        BOOST_FOREACH(size_t id, ids)
            organisms.push_back(evolution::organism(solution_config(id)));

        // mutate the solutions. We don't want to run the old ones again
        const double mutation_rate = 0.2;
        size_t generation;
        evolution::reproducer repro(pred_site_id_, mutation_rate, generation);

        organisms = repro.breed(organisms, population.size());

        population.clear();
        BOOST_FOREACH(evolution::organism &orga, organisms)
            population.push_back(shared_ptr<solution_config>(new solution_config(orga.genes())));
    }
    else
    {
        trans.commit();
        report(DEBUGING) << "we don't have enough solutions in the database from previous runs, starting from scratch";
    }

    // add in the best solutions from other flying sites
    pqxx::transaction<> trans2(flightpred_db::get_conn(), "get site names");
    sstr.str("");
    sstr << "SELECT generation FROM trained_solutions WHERE pred_site_id=" << pred_site_id_ << " ORDER BY generation DESC LIMIT 1";
    res = trans2.exec(sstr.str());
    size_t generation = 0;
    if(res.size())
    {
        res[0][0].to(generation);
        ++generation;
    }

    sstr.str("");
    sstr << "SELECT pred_site_id, site_name FROM pred_sites WHERE pred_site_id <> " << pred_site_id_;
    res = trans2.exec(sstr.str());
    if(!res.size())
        throw std::invalid_argument("no sites found");
    vector<size_t> sites;
    for(size_t i=0; i<res.size(); ++i)
    {
        size_t sid;
        res[i][0].to(sid);
        sites.push_back(sid);
    }

    BOOST_FOREACH(size_t site_id, sites)
    {
        report(DEBUGING) << "Adding the best solutions from site " << site_id;

        sstr.str("");
        sstr << "SELECT train_sol_id, configuration, validation_error, train_time, generation FROM trained_solutions "
             << "WHERE train_time <= 50 AND " << "pred_site_id=" << site_id << " "
             << "ORDER BY validation_error ASC, generation DESC LIMIT 5";
        res = trans2.exec(sstr.str());
        if(!res.size())
            continue;

        for(size_t i=0; i<res.size() && i<5; ++i)
        {
            size_t solution_id;
            res[i]["train_sol_id"].to(solution_id);
            string config;
            res[i]["configuration"].to(config);

            population.push_back(shared_ptr<solution_config>(new solution_config(site_name_, config, generation)));
        }
    }

    return population;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief test how well a solution can predict the flights.
    @return the total error in km */
double solution_manager::test_fitness(const solution_config &sol)
{
    report(VERBOSE) << "solution_manager::test_fitness(" << sol.get_short_description() <<  ")";
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
    double sum_err = 0.0, sum_km = 0.0, sum_km_pred = 0.0;
    size_t validation_count = 0;
    for(bgreg::day_iterator dit(date_extremes_.begin()); *dit <= date_extremes_.end(); ++dit)
    {
        if(used_for_validation(*dit))
        {
            const vector<double> &samples = get_sample(sol.get_weather_feature_desc(), *dit);
            const double predval   = sol.get_decision_function(eval_name)->eval(samples);
            const double predval_p = predval > 0.0 ? predval : 0.0; // to catch nan
            const double & realval = max_distances_[*dit];
            const double err = fabs(realval - predval_p);
            sum_err     += err;
            sum_km      += realval;
            sum_km_pred += predval_p;
            ++validation_count;
        }
    }
    report(INFO) << "validation result for : " << sol.get_short_description() << std::endl
                 << " gen " << sol.get_generation()
                 << " total error in km : " << sum_err <<  std::endl
                 << " total flown km    : " << sum_km << std::endl
                 << " total predicted km : " << sum_km_pred << std::endl
                 << " validated with " << validation_count << " days" << std::endl;

    // increase the error number if certain unwanted conditions apply
    if(sum_km_pred < sum_km / 10.0)
        sum_err += 10 * sum_km;

 //   if(traintime > 0.7 * )

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
bool solution_manager::used_for_training(const bgreg::date &day) const
{
    if(day.day_of_week() != 0) // sunday)
        return false;

    if(ignored_days_.find(day) != ignored_days_.end() || ignored_days_.find(day - bgreg::days(1)) != ignored_days_.end() || ignored_days_.find(day + bgreg::days(1)) != ignored_days_.end())
        return false;

    return true;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
bool solution_manager::used_for_validation(const bgreg::date &day) const
{
    if(day.day_of_week() != 6) // saturday
        return false;

    if(ignored_days_.find(day) != ignored_days_.end() || ignored_days_.find(day - bgreg::days(1)) != ignored_days_.end() || ignored_days_.find(day + bgreg::days(1)) != ignored_days_.end())
        return false;

    return true;
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
const set<bgreg::date> solution_manager::get_ignored_days(bool onlyConfirmed)
{
    set<bgreg::date> ignored_days;

    try
    {
        pqxx::transaction<> trans(flightpred_db::get_conn(), "fill_max_distance_cache");
        std::stringstream sstr;
        sstr << "SELECT pred_day FROM pred_ignore";
        if(onlyConfirmed)
            sstr << " WHERE confirmed=true";
        pqxx::result res = trans.exec(sstr.str());

        for(size_t i=0; i<res.size(); ++i)
        {
            string datestr;
            res[i][0].to(datestr);
            bgreg::date day(bgreg::from_string(datestr));
            ignored_days.insert(day);
        }
    }
    catch (std::exception &ex)
    {
        report(WARN) << "Failed to load ignored days. Using all possible dates: " << ex.what();
    }

    return ignored_days;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
std::auto_ptr<solution_config> solution_manager::load_best_solution(const bool onlyFullyTrained, const double maxTrainSec)
{
    report(VERBOSE) << "solution_manager::load_best_solution(" << std::ios::boolalpha << onlyFullyTrained << ", " << maxTrainSec <<  ")";
    pqxx::transaction<> trans(flightpred_db::get_conn(), "load_best_solution");

    std::stringstream sstr;
    sstr << "SELECT train_sol_id, configuration, validation_error, train_time, generation FROM trained_solutions WHERE ";
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
void solution_manager::export_solution(const bfs::path &backup_dir)
{
    bfs::create_directories(backup_dir);
    bfs::path outfile(backup_dir / (site_name_ + ".flightpred"));
	bfs::ofstream ofs(outfile, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	if(!ofs.good())
        throw std::runtime_error("could not write to " + outfile.string());
	boost::archive::text_oarchive oa(ofs);

    // information about the flying site
	oa << site_name_;

	pqxx::transaction<> trans(flightpred_db::get_conn(), "solution_manager::export_solution");
    std::stringstream sstr;
    sstr << "SELECT AsText(location) as loc, country, pred_site_id FROM pred_sites "
         << "WHERE site_name='" << site_name_ << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("site not found : " + site_name_);
    string country;
    res[0]["country"].to(country);
    oa << country;
    string locstr;
    res[0]["loc"].to(locstr);
    point_ll_deg location;
    boost::geometry::read_wkt(locstr, location);
	oa << location;
	size_t pred_site_id;
	res[0]["pred_site_id"].to(pred_site_id);

	// trained solutions
	sstr.str("");
	sstr << "SELECT * FROM trained_solutions "
         << "WHERE pred_site_id=" << pred_site_id;
    res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("could not query details about trained solutions");
    const size_t num_trained_solution = res.size();
    oa << num_trained_solution;
    for(size_t i=0; i<num_trained_solution; ++i)
    {
        string config;
        res[i]["configuration"].to(config);
        oa << config;

        double validation_error;
        res[i]["validation_error"].to(validation_error);
        oa << validation_error;

        double train_time;
        res[i]["train_time"].to(train_time);
        oa << train_time;

        double train_time_prod;
        res[i]["train_time_prod"].to(train_time_prod);
        oa << train_time_prod;

        size_t generation;
        res[i]["generation"].to(generation);
        oa << generation;

        size_t num_samples;
        res[i]["num_samples"].to(num_samples);
        oa << num_samples;

        size_t num_samples_prod;
        res[i]["num_samples_prod"].to(num_samples_prod);
        oa << num_samples_prod;

        size_t num_features;
        res[i]["num_features"].to(num_features);
        oa << num_features;
    }
    trans.commit();

    // register all derived algoritms that are in use
    oa.register_type<lm_dlib_rvm <dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > >();
    oa.register_type<lm_dlib_rvm <dlib::sigmoid_kernel     <dlib::matrix<double, 0, 1> > > >();
    oa.register_type<lm_dlib_rvm <dlib::polynomial_kernel  <dlib::matrix<double, 0, 1> > > >();
    oa.register_type<lm_dlib_krls<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > >();

	// serialize the solution itself
    std::auto_ptr<solution_config> sol = load_best_solution(true);
    solution_config *sc = sol.get();
	oa << sc;

    report(INFO) << "solution_config sucesfully exported to : " << outfile.string();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void solution_manager::import_solution(const bfs::path &backup_dir)
{
    bfs::path infile(backup_dir / (site_name_ + ".flightpred"));
    if(!bfs::exists(infile))
        throw std::runtime_error("file not found : " + infile.string());
    bfs::ifstream ifs(infile, std::ios_base::in | std::ios_base::binary);
	if(!ifs.good())
        throw std::runtime_error("file not found : " + infile.string());

	boost::archive::text_iarchive ia(ifs);
    // read class state from archive
    string site_name;
	ia >> site_name;
	if(site_name != site_name_)
        throw std::runtime_error("wrong site : " + site_name + " != " + site_name_);

    string country;
    ia >> country;
    point_ll_deg location;
	ia >> location;

	pqxx::transaction<> trans(flightpred_db::get_conn(), "solution_manager::import_solution");
    pqxx::result res = trans.exec("SELECT site_name, pred_site_id, AsText(location) as loc FROM pred_sites");
    int site_id = -1;
    for(size_t i=0; i<res.size(); ++i)
    {
        size_t siteid;
        res[i]["pred_site_id"].to(siteid);
        string sitenam;
        res[i]["site_name"].to(sitenam);
        if(sitenam == site_name)
        {
            site_id = siteid;
            break;
        }
        string locstr;
        res[i]["loc"].to(locstr);
        point_ll_deg location;
        boost::geometry::read_wkt(locstr, location);

        // todo : use with different name but close location
    }

    if(site_id < 0)
    {
        // register the prediction site
        static const size_t PG_SIR_WGS84 = 4326;
        std::stringstream sstr;
        sstr << "INSERT INTO pred_sites (site_name, country, location) VALUES ('"
             << site_name << "', '" << country << "', "
             << "ST_GeomFromText('" << boost::geometry::make_wkt(location) << "', " << PG_SIR_WGS84 << "))";
        trans.exec(sstr.str());

        sstr.str("");
        sstr << "SELECT pred_site_id FROM pred_sites "
             << "WHERE site_name='" << site_name << "'";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            throw std::runtime_error("site creation failed : " + site_name);
        res[0]["pred_site_id"].to(site_id);
    }

    // trained solutions
    size_t num_trained_solution = 0;
    ia >> num_trained_solution;
    for(size_t i=0; i<num_trained_solution; ++i)
    {
        string config;
        ia >> config;
        double validation_error;
        ia >> validation_error;
        double train_time;
        ia >> train_time;
        double train_time_prod;
        ia >> train_time_prod;
        size_t generation;
        ia >> generation;
        size_t num_samples;
        ia >> num_samples;
        size_t num_samples_prod;
        ia >> num_samples_prod;
        size_t num_features;
        ia >> num_features;

        std::stringstream sstr;
        sstr << "SELECT train_sol_id FROM trained_solutions WHERE "
             << "pred_site_id="  << site_id << " AND "
             << "configuration='" << config  << "'";
        res = trans.exec(sstr.str());
        if(res.size())
        {
            // todo : option to override the numbers
            continue;
        }

        sstr.str("");
        sstr << "INSERT INTO trained_solutions (pred_site_id, generation, configuration, validation_error, train_time, "
             << "train_time_prod, num_samples, num_samples_prod, num_features) VALUES "
             << "(" << site_id << ", " << generation << ", '" << config << "', " << validation_error << ", "
             << train_time << ", " << train_time_prod << ", " << num_samples << ", " << num_samples_prod << ", " << num_features << ")";
        trans.exec(sstr.str());
    }
    trans.commit();

    // register all derived algoritms
    ia.register_type<lm_dlib_rvm <dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > >();
    ia.register_type<lm_dlib_rvm <dlib::sigmoid_kernel     <dlib::matrix<double, 0, 1> > > >();
    ia.register_type<lm_dlib_rvm <dlib::polynomial_kernel  <dlib::matrix<double, 0, 1> > > >();
    ia.register_type<lm_dlib_krls<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > >();

    solution_config *sol = 0;
    ia >> sol;
    // the de-serialization of the solution_config wrote it to the db already...


    report(INFO) << "solution_config sucesfully imported from : " << infile.string();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A



