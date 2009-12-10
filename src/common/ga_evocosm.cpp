// flightpred
#include "common/ga_evocosm.h"
// evocosm
#include <libevocosm/roulette.h>
// std lib
#include <iostream>
#include <stdexcept>

using namespace flightpred;
using namespace flightpred::evolution;
using std::vector;
using std::cout;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// mutate a set of organisms
void mutator::mutate(vector<organism> &population)
{
    for(vector<organism>::iterator solution = population.begin(); solution != population.end(); ++solution)
    {
/*
        for (vector<double>::iterator arg = solution->genes().begin(); arg != solution->genes().end(); ++arg)
        {
            if (g_random.get_real() <= m_mutation_rate)
                *arg = g_evoreal.mutate(*arg);
        }
*/
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// create children
vector<organism> reproducer::breed(const vector<organism> &old_population, size_t p_limit)
{
    // result
    vector<organism> children;

    // construct a fitness wheel
    vector<double> wheel_weights;
    for(vector<organism>::const_iterator solution = old_population.begin(); solution != old_population.end(); ++solution)
        wheel_weights.push_back(solution->fitness());

    libevocosm::roulette_wheel fitness_wheel(wheel_weights);

    // create children
    while(p_limit > 0)
    {
        // clone an existing organism as a child
        size_t genes_index = fitness_wheel.get_index();
        organism child = old_population[genes_index];
/*
        vector<double> & child_genes = child.genes();

        // do we crossover?
        if (g_random.get_real() <= m_crossover_rate)
        {
            // select a second parent
            size_t second_index = genes_index;

            while (second_index == genes_index)
                second_index = fitness_wheel.get_index();

            const vector<double> & second_genes = a_population[second_index].genes();

            // reproduce
            for (size_t n = 0; n < child_genes.size(); ++n)
                child_genes[n] = g_evoreal.crossover(child_genes[n],second_genes[n]);
        }
*/
        // add child to new population
        children.push_back(child);

        // one down, more to go?
        --p_limit;
    }

    // outa here!
    return children;
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
                                       t_init * a_init,
                                       size_t population,
                                       double mutation_rate,
                                       size_t iterations)
  : mutator_(mutation_rate),
    reproducer_(),
    scaler_(),
    migrator_(),
    selector_(population / 10),
    reporter_(),
    evocosm_(NULL),
    iterations_(iterations),
    init_(a_init),
    eval_fitness_(eval_fitness),
    listener_()
{
    if (a_init == NULL)
        throw std::runtime_error("NULL organism initializer in optimizer construction");

    evocosm_ = new libevocosm::evocosm<organism, landscape>(listener_,
                                                 population,
                                                 1,
                                                 0,
                                                 1,
                                                 mutator_,
                                                 reproducer_,
                                                 scaler_,
                                                 migrator_,
                                                 selector_,
                                                 reporter_,
                                                 *this,
                                                 *this);
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
        evocosm_->run_generation(true,dummy);
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// Organism factory
organism optimizer::create()
{
    return init_();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void optimizer::append(vector<organism> &population, size_t a_size)
{
    // fill remaining population with random values
    for(size_t i = 0; i < a_size; ++i)
        population.push_back(init_());
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

