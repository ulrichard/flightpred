// flightpred
#include "ga_evocosm.h"
#include "flightpred_globals.h"
#include "reporter.h"
// evocosm
#include <libevocosm/roulette.h>
// postgre
#include <pqxx/pqxx>
// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/io/wkt/read_wkt.hpp>
// boost
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
//#include <boost/lambda/core.hpp>
//#include <boost/lambda/bind.hpp>
//#include <boost/lambda/if.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random.hpp>
// std lib
#include <iostream>
#include <stdexcept>

using namespace flightpred;
using namespace flightpred::evolution;
using namespace flightpred::reporting;
//using namespace boost::lambda;
//namespace bll = boost::lambda;
using boost::shared_ptr;
using boost::lexical_cast;
using std::string;
using std::vector;
using std::cout;

// static members;
libevocosm::evoreal mutator::evoreal_;
libevocosm::evoreal reproducer::evoreal_(0.0, 3.0, 97.0);
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
reproducer::reproducer(const size_t pred_site_id, const double mutation_rate, size_t &current_generation)
    : pred_site_id_(pred_site_id), mutation_rate_(mutation_rate), current_generation_(current_generation)
{
    report(DEBUGING) << "reproducer::reproducer(" << pred_site_id << ", " << mutation_rate << ", " << current_generation << ") -> " << current_generation_;
    // get the location of the current site
    pqxx::transaction<> trans(flightpred_db::get_conn(), "reproducer::reproducer");
    std::stringstream sstr;
    sstr << "SELECT AsText(location) AS loc FROM pred_sites WHERE pred_site_id=" << pred_site_id_;
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("prediction site not found");
    string tmploc;
    res[0][0].to(tmploc);
    boost::geometry::read_wkt(tmploc, site_location_);

    // load all known configurations into the set
    sstr.str("");
    sstr << "SELECT configuration FROM trained_solutions "
         << "WHERE pred_site_id=" << pred_site_id_;
    res = trans.exec(sstr.str());
    for(size_t i=0; i<res.size(); ++i)
    {
        string sol_conf;
        res[i][0].to(sol_conf);
        solution_descriptions_.insert(sol_conf);
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// create children
vector<organism> reproducer::breed(const vector<organism> &old_population, size_t p_limit)
{
    report(INFO) << "reproducer::breed generation " << current_generation_ << "+1  count : " << p_limit;
    // result
    vector<organism> children;

    // take some of the best organisms so far in the system
    report(DEBUGING) << "loading the " << (p_limit / 10) << " best organisms from the db";
    pqxx::transaction<> trans(flightpred_db::get_conn(), "reproducer::breed");
    std::stringstream sstr;
    sstr << "SELECT train_sol_id FROM trained_solutions "
         << "WHERE pred_site_id=" << pred_site_id_ << " "
         << "AND train_time <= 50.0 "
         << "AND validation_error > 0.0 "
         << "ORDER BY validation_error ASC "
         << "LIMIT " << p_limit / 10;
    pqxx::result res = trans.exec(sstr.str());
    vector<size_t> ids;
    for(size_t i=0; i<res.size(); ++i, --p_limit)
    {
        size_t train_sol_id;
        res[i][0].to(train_sol_id);
        ids.push_back(train_sol_id);
    }
    trans.commit();
    report(DEBUGING) << "mutate them";
    BOOST_FOREACH(size_t train_sol_id, ids)
    {
        solution_config sol(train_sol_id);
        solution_descriptions_.insert(sol.get_description());
        while(solution_descriptions_.find(sol.get_description()) != solution_descriptions_.end())
            sol = make_mutated_clone(sol);

        // add child to new population
        children.push_back(organism(sol));
        solution_descriptions_.insert(sol.get_description());
    }

    report(DEBUGING) << "get the remaining organisms from the previous generation and mutate them";
    // construct a fitness wheel
    vector<double> wheel_weights;
    for(vector<organism>::const_iterator it = old_population.begin(); it != old_population.end(); ++it)
        wheel_weights.push_back(1.0 / it->fitness());
    libevocosm::roulette_wheel fitness_wheel(wheel_weights);

    // create children
    for(; p_limit > 0; --p_limit)
    {
        report(DEBUGING) << "left to go : " << p_limit;
        // clone an existing organism as a child
        size_t genes_index = fitness_wheel.get_index();
        report(DEBUGING) << "roulette wheel selected index " << genes_index << " out of " <<  old_population.size();
        genes_index = std::min(genes_index, old_population.size() - 1);
        const organism &child = old_population[genes_index];

        solution_config sol(child.genes());
        report(DEBUGING) << sol.get_short_description();
        solution_descriptions_.insert(sol.get_description());
        while(solution_descriptions_.find(sol.get_description()) != solution_descriptions_.end())
            sol = make_mutated_clone(sol);

        // add child to new population
        children.push_back(organism(sol));
        solution_descriptions_.insert(sol.get_description());
    }

//    assert(children.size() == old_population.size()); // for our case where we have no pre-selected survivors
    return children;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
solution_config reproducer::make_mutated_clone(const solution_config &src)
{
    string algo_desc = src.get_algorithm_name(true);
    boost::trim(algo_desc);
    std::stringstream sstr;

    // first the algorithm and kernel names
    report(DEBUGING) << "reproducer::make_mutated_clone : " << algo_desc;
    boost::smatch regxmatch;
    if(!boost::regex_search(algo_desc, regxmatch, boost::regex("\\w+\\(\\w+\\(\\s*")))
        throw std::invalid_argument("failed to mutate the solution description : " + algo_desc);
    sstr << regxmatch[0];
    algo_desc = algo_desc.substr(regxmatch[0].length());

    // second the kernel parameters
    boost::regex rgxdbl("\\d+((\\.\\d+)?(e[+-]\\d{1,3})?)?\\s*");
    while(boost::regex_search(algo_desc, regxmatch, rgxdbl))
    {
        string nbr = regxmatch[0];
        algo_desc = algo_desc.substr(nbr.length());
//        report(DEBUGING) << "old : " << algo_desc;
        boost::trim(nbr);
        double val = lexical_cast<double>(nbr);
        const double rval = (g_random.get_rand() % 1000) / 1000.0;
        if(rval <= mutation_rate_)
        {
            double newval = 0.0;
            while(newval <= 0.0 || newval >= 0.99)
                newval = evoreal_.mutate(val);
            val = newval;
        }

        val = std::min<double>(val, 1.0 - std::numeric_limits<float>::min());
        val = std::max<double>(val, 0.0 + std::numeric_limits<float>::min());

        sstr << val << " ";
 //       report(DEBUGING) << "new : " << sstr.str();
        if(algo_desc[0] == ')')
            break;
    }
    sstr << ")";
//    report(DEBUGING) << "new : " << sstr.str();

    // third an optional algorithm parameter
    assert(algo_desc[0] == ')');
    algo_desc = algo_desc.substr(1, algo_desc.length() - 2);
    if(boost::regex_search(algo_desc, regxmatch, rgxdbl))
    {
        string nbr = regxmatch[0];
        boost::trim(nbr);
        double val = lexical_cast<double>(nbr);
        const double rval = (g_random.get_rand() % 1000) / 1000.0;
        if(rval <= mutation_rate_)
            val = evoreal_.mutate(val);

        sstr << val;
        algo_desc = algo_desc.substr(nbr.length());
    }
    sstr << ")  ";
//    report(DEBUGING) << "new : " << sstr.str();

    // then mutate the features
    std::set<features_weather::feat_desc> features =  src.get_weather_feature_desc();

    static boost::mt19937 rng;

    // add some new features
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >  randgen(rng, boost::uniform_int<>(0, features.size() / 10));
    const size_t target_count_add = features.size() + randgen();
    report(DEBUGING) << "add some new features from " << features.size() << " up to " << target_count_add;
    while(features.size() < target_count_add)
        features.insert(features_weather::get_random_feature(site_location_));

    // remove some features
    const size_t target_count_sub = features.size() - randgen();
    report(DEBUGING) << "drop some features from " << features.size() << " down to " << target_count_sub;
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >  randgen_ia(rng, boost::uniform_int<>(0, features.size() - 1));
    while(features.size() > target_count_sub)
    {
        std::set<features_weather::feat_desc>::iterator it = features.begin();
        std::advance(it, randgen_ia() % features.size());
        features.erase(it);
    }

    // mutate some features
    report(DEBUGING) << "mutate some features";
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >  randgen_mut(rng, boost::uniform_int<>(0, 100));
    BOOST_FOREACH(const features_weather::feat_desc &feat, features)
    {
        if(randgen_mut() >= mutation_rate_ * 100)
            sstr << features_weather::mutate_feature(feat);
        else
            sstr << feat;
        sstr << " ";
    }

    return solution_config(src.get_site_name(), sstr.str(), current_generation_ + 1);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief Fitness testing
    Tests each chromosome by calling the fitness function provided in the constructor.
    \param population - Population undergoing fitness testing
    \return Average fitness of population  */
double landscape::test(std::vector<organism> &population) const
{
   double result = 0.0;

    for(vector<organism>::iterator solution = population.begin(); solution != population.end(); ++solution)
    {
        solution->fitness() = eval_fitness_(solution->genes());
        result += solution->fitness();
    }

    return result / population.size();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// say something about a population
bool reporter::report(const vector<vector<organism> > &populations, size_t iteration, double & fitness, bool finished)
{
    // search for best chromosome; there will be only one population for this application
    organism best = *(populations.begin()->begin());

    for(vector<organism>::const_iterator solution = populations.begin()->begin(); solution != populations.begin()->end(); ++solution)
    {
        if(solution->fitness() > best.fitness())
            best = *solution;
    }

    // return best fitness via argument (actually ignored by calling function right now)
    fitness = best.fitness();

    // display best solution
//    size_t n = 0;

    cout.precision(15);

//    for(vector<double>::iterator arg = best.genes().begin(); arg != best.genes().end(); ++arg, ++n)
//        cout << "[" << n << "] " << *arg << "\n";

//    report(VERBOSE) << std::setfill(' ') << "fitness = " << std::setw(2) << best.fitness();

    return true;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// constructor
optimizer::optimizer(boost::function<double(const solution_config&)> eval_fitness,
                     boost::function<organism(void)> a_init,
                     boost::function<std::vector<shared_ptr<solution_config> >(void)> init_population,
                     size_t population,
                     double mutation_rate,
                     size_t iterations,
                     size_t pred_site_id)
  : current_generation_(0),
    mutator_(mutation_rate),
    reproducer_(pred_site_id, mutation_rate, current_generation_),
    scaler_(),
    migrator_(),
    selector_(population / 10),
    reporter_(),
    evocosm_(NULL),
    iterations_(iterations),
    init_(a_init),
    init_population_(init_population),
    eval_fitness_(eval_fitness),
    listener_()
{
    report(DEBUGING) << "optimizer::optimizer() gen " << current_generation_;

    evocosm_ = new libevocosm::evocosm<organism, landscape>(listener_,
                                                 population,    // population size
                                                 1,             // number of populations
                                                 0,             // number_of_unique_landscapes
                                                 1,             // number_of_common_landscapes
                                                 mutator_,
                                                 reproducer_,
                                                 scaler_,
                                                 migrator_,
                                                 selector_,
                                                 reporter_,
                                                 *this,         // organism factory
                                                 *this);        // landscape factory
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void optimizer::run()
{
    // continue for specified number of iterations
    const size_t last_generation = current_generation_ + iterations_;
    for( ; current_generation_ <= last_generation; ++current_generation_)
    {
        // display generation number
        report(INFO) << "\ngeneration " << current_generation_ << std::endl;

        // run a generation
        double dummy;
        evocosm_->run_generation(true, dummy);
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// Organism factory
organism optimizer::create()
{
    assert(!"check to see if we get here without being called from append()");
    return init_();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void optimizer::append(vector<organism> &population, size_t a_size)
{
    typedef shared_ptr<solution_config> shared_solution_config;
    vector<shared_solution_config> sol_configs = init_population_();
    current_generation_ = sol_configs.back()->get_generation();

    BOOST_FOREACH(shared_solution_config sol, sol_configs)
        population.push_back(organism(*sol));

    assert(population.size() >= a_size);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// Landscape factory
landscape optimizer::generate()
{
    return landscape(eval_fitness_, listener_);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

