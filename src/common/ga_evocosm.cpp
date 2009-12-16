// flightpred
#include "common/ga_evocosm.h"
#include "common/flightpred_globals.h"
// evocosm
#include <libevocosm/roulette.h>
// postgre
#include <pqxx/pqxx>
// boost
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
//#include <boost/lambda/core.hpp>
//#include <boost/lambda/bind.hpp>
//#include <boost/lambda/if.hpp>
// std lib
#include <iostream>
#include <stdexcept>

using namespace flightpred;
using namespace flightpred::evolution;
//using namespace boost::lambda;
//namespace bll = boost::lambda;
using boost::shared_ptr;
using boost::lexical_cast;
using std::string;
using std::vector;
using std::cout;

// static members;
libevocosm::evoreal mutator::evoreal_;
libevocosm::evoreal reproducer::evoreal_;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// mutate a set of organisms
void mutator::mutate(vector<organism> &population)
{
    // in our flightprediction, the mutation happens during the breeding
/*
    std::cout << "mutator::mutate" << std::endl;

    for(vector<organism>::iterator it = population.begin(); it != population.end(); ++it)
    {
        solution_config &solconf = it->genes();
        boost::function<double(double)> func = boost::bind(&mutator::mutate_value, this, ::_1);
        solconf.mutate(mutation_rate_, func);


        for (vector<double>::iterator arg = solution->genes().begin(); arg != solution->genes().end(); ++arg)
        {
            if (g_random.get_real() <= m_mutation_rate)
                *arg = g_evoreal.mutate(*arg);
        }
    }
*/
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
double mutator::mutate_value(double oldval)
{
    const double rval = (g_random.get_rand() % 1000) / 1000.0;
    if(rval <= mutation_rate_)
        return evoreal_.mutate(oldval);
    return oldval;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
reproducer::reproducer(const size_t pred_site_id, const double mutation_rate)
    : pred_site_id_(pred_site_id), mutation_rate_(mutation_rate)
{
    pqxx::transaction<> trans(flightpred_db::get_conn(), "reproducer::reproducer");
    std::stringstream sstr;
    sstr << "SELECT configuration FROM trained_solutions "
         << "WHERE pred_site_id=" << pred_site_id_;
    pqxx::result res = trans.exec(sstr.str());
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
    std::cout << "reproducer::breed" << std::endl;
    // result
    vector<organism> children;

    // take some of the best organisms so far in the system
    pqxx::transaction<> trans(flightpred_db::get_conn(), "reproducer::breed");
    std::stringstream sstr;
    sstr << "SELECT train_sol_id FROM trained_solutions "
         << "WHERE pred_site_id=" << pred_site_id_ << " "
         << "AND train_time <= 50.0 "
         << "AND validation_error > 0.0 "
         << "ORDER BY validation_error ASC "
         << "LIMIT " << p_limit / 10;
    pqxx::result res = trans.exec(sstr.str());
    for(size_t i=0; i<res.size(); ++i, --p_limit)
    {
        size_t train_sol_id;
        res[i][0].to(train_sol_id);

        solution_config sol(train_sol_id);
        while(solution_descriptions_.find(sol.get_description()) != solution_descriptions_.end())
            sol = make_mutated_clone(sol);

        // add child to new population
        children.push_back(organism(sol));
        solution_descriptions_.insert(sol.get_description());
    }

    // construct a fitness wheel
    vector<double> wheel_weights;
    for(vector<organism>::const_iterator solution = old_population.begin(); solution != old_population.end(); ++solution)
        wheel_weights.push_back(1.0 / solution->fitness());
    libevocosm::roulette_wheel fitness_wheel(wheel_weights);

    // create children
    for(; p_limit > 0; --p_limit)
    {
        // clone an existing organism as a child
        size_t genes_index = fitness_wheel.get_index();
        organism child = old_population[genes_index];

        solution_config sol(child.genes());
        while(solution_descriptions_.find(sol.get_description()) != solution_descriptions_.end())
            sol = make_mutated_clone(sol);

        // add child to new population
        children.push_back(organism(sol));
        solution_descriptions_.insert(sol.get_description());
    }

    assert(children.size() == old_population.size()); // for our case where we have no pre-selected survivors
    return children;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
solution_config reproducer::make_mutated_clone(const solution_config &src)
{
    string algo_desc = src.get_algorithm_name(true);
    boost::trim(algo_desc);
    std::stringstream sstr;

//    boost::regex regx("\\w+\\(\\w+\\((\\d+\\.\\d+\\s*){1,3}\\)(\\s*\\d+\\.\\d+)?\\s*\\)");

    // first the algorithm and kernel names
    boost::smatch regxmatch;
    if(!boost::regex_search(algo_desc, regxmatch, boost::regex("\\w+\\(\\w+\\(\\s*")))
        throw std::invalid_argument("failed to mutate the solution description : " + algo_desc);
    sstr << regxmatch[0];
    algo_desc = algo_desc.substr(regxmatch[0].length());

    // second the kernel parameters
    boost::regex rgxdbl("\\d+(\\.\\d+)?\\s*");
    while(boost::regex_search(algo_desc, regxmatch, rgxdbl))
    {
        string nbr = regxmatch[0];
        algo_desc = algo_desc.substr(nbr.length());
        boost::trim(nbr);
        double val = lexical_cast<double>(nbr);
        const double rval = (g_random.get_rand() % 1000) / 1000.0;
        if(rval <= mutation_rate_)
            val = evoreal_.mutate(val);
        sstr << val << " ";

        std::cout << algo_desc << std::endl;
    }
    sstr << ")";

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

//    std::cout << src.get_algorithm_name(true) << "  ->  " << sstr.str() << std::endl;

    // then mutate the features
    std::set<features_weather::feat_desc> features =  src.get_weather_feature_desc();

    // mutations will include : drop, modify, add

    std::copy(features.begin(), features.end(), std::ostream_iterator<features_weather::feat_desc>(sstr, " "));

//    return solution_config(src.get_site_name(), sstr.str());
    std::cout << sstr.str().substr(0, 50) << std::endl << std::endl;

    solution_config soco(src.get_site_name(), sstr.str());
    std::cout << sstr.str().substr(0, 50) << std::endl << std::endl;
    return soco;
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
    size_t n = 0;

    cout.precision(15);

//    for(vector<double>::iterator arg = best.genes().begin(); arg != best.genes().end(); ++arg, ++n)
//        cout << "[" << n << "] " << *arg << "\n";

    cout << std::setfill(' ') << "fitness = " << std::setw(2) << best.fitness() << std::endl;

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
  : mutator_(mutation_rate),
    reproducer_(pred_site_id, mutation_rate),
    scaler_(),
    migrator_(),
    selector_(population / 10),
    reporter_(),
    evocosm_(NULL),
    iterations_(iterations),
    init_(a_init),
    eval_fitness_(eval_fitness),
    init_population_(init_population),
    listener_()
{
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
    for (size_t count = 1; count <= iterations_; ++count)
    {
        // display generation number
        cout << "\ngen " << count << std::endl;

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

    BOOST_FOREACH(shared_solution_config sol, sol_configs)
        population.push_back(organism(*sol));

    assert(population.size() == a_size);
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

