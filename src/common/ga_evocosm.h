#ifndef GA_EVOCOSM_H_INCLUDED
#define GA_EVOCOSM_H_INCLUDED

// flightpred
#include "common/solution_config.h"
// evocosm
#include <libevocosm/organism.h>
#include <libevocosm/mutator.h>
#include <libevocosm/landscape.h>
#include <libevocosm/reporter.h>
#include <libevocosm/reproducer.h>
#include <libevocosm/evocosm.h>
// boost
#include <boost/function.hpp>
// std lib
#include <algorithm>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
namespace evolution
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class organism : public libevocosm::organism<solution_config>
{
public:
    explicit organism(const solution_config &src) : libevocosm::organism<solution_config>(src) { }
    organism(const organism &src) : libevocosm::organism<solution_config>(src) { }

    virtual ~organism() { }

    organism & operator=(const organism &src)
    {
        libevocosm::organism<solution_config>::operator=(src);
        return *this;
    }

    /** organisms less error come before ones with bigger error. */
    virtual bool operator<(const libevocosm::organism<solution_config> &rhs) const
    {
        return (m_fitness < rhs.fitness());
    }
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class mutator : public libevocosm::mutator<organism>
{
public:
    mutator(double mutation_rate) : mutation_rate_(mutation_rate)
    {
        mutation_rate_ = std::min(mutation_rate_, 1.0);
        mutation_rate_ = std::max(mutation_rate_, 0.0);
    }
    mutator(const mutator &src) : mutation_rate_(src.mutation_rate_) { }
    virtual ~mutator() { }

    mutator & operator=(const mutator &src)
    {
        mutation_rate_ = src.mutation_rate_;
        return *this;
    }

    double mutation_rate() const { return mutation_rate_; }

    virtual void mutate(std::vector<organism> &population);

private:
    double mutation_rate_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class reproducer : public libevocosm::reproducer<organism>
{
public:
    reproducer() {  }
    reproducer(const reproducer &src) { }
    virtual ~reproducer()  { }
    reproducer & operator=(const reproducer &source)
    {   return *this;   }

    //! Reproduction for solutions
    /*!
        Breeds new solutions, by cloning or the combination of elements from parent organisms.
        In the flightprediction system we don't use sexual reproduction, meaning crossover, but only cloning with mutations.
        \param a_population - A population of solutions
        \param p_limit - Maximum number of children
        \return A vector containing new "child" chromosomes
    */
    virtual std::vector<organism> breed(const std::vector<organism> &old_population, size_t p_limit);

private:
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief Defines the test for a population of solutions
    A "fitness" landscape defines the environment in which an organism
    competes for survival.*/
class landscape : public libevocosm::landscape<organism>
{
public:
    landscape(boost::function<double(const solution_config&)> eval_fitness, libevocosm::listener &lstnr)
        : libevocosm::landscape<organism>(lstnr), eval_fitness_(eval_fitness) { }
    landscape(const landscape &org)
        : libevocosm::landscape<organism>(org), eval_fitness_(org.eval_fitness_) { }
    virtual ~landscape() { }
    landscape & operator=(const landscape &rhs)
    {
        libevocosm::landscape<organism>::operator=(rhs);
        eval_fitness_ = rhs.eval_fitness_;
        return *this;
    }

    /** performs fitness testing */
    virtual double test(organism &orgn, bool verbose = false) const
    {
        orgn.fitness() = eval_fitness_(orgn.genes());
        return orgn.fitness();
    }
    /** performs fitness testing */
    virtual double test(std::vector<organism> &population) const;

private:
    boost::function<double(const solution_config&)> eval_fitness_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief Reports the state of a population of solutions.
    A simple reporter for diaplying information about the populations
    as it evolves.*/
class reporter : public libevocosm::reporter<organism, landscape>
{
public:
    reporter() : libevocosm::reporter<organism, landscape>(listener_) { }

    /** @brief say something about a population */
    virtual bool report(const std::vector<std::vector<organism> > &population, size_t iteration, double &fitness, bool finished = false);

protected:
    libevocosm::listener_stdout listener_;

};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief A generic optimizer
    Using instances of the other classes, this class binds together the pieces to
    create a complete optimizer. A user of this class defines two functions
    -- a solution initializer and a fitness test -- that define the target problem.*/
class optimizer : protected libevocosm::organism_factory<organism>, protected libevocosm::landscape_factory<landscape>
{
private:
    // objects that define the characteristics of the genetic algorithm
    mutator                                mutator_;
    reproducer                             reproducer_;
    libevocosm::null_scaler<organism>      scaler_;
    libevocosm::null_migrator<organism>    migrator_;
    libevocosm::elitism_selector<organism> selector_;
    reporter                               reporter_;

    // the evocosm binds it all together; kinda like the "one ring" Bilbo found!
    libevocosm::evocosm<organism, landscape> * evocosm_;

    // number of iterations to run
    const size_t iterations_;

    // a function that provides initialized, random solutions
    boost::function<organism(void)> init_;

    // a function that generates the initial population
    boost::function<std::vector<boost::shared_ptr<solution_config> >(void)> init_population_;

    // the function to be optimized
    boost::function<double(const solution_config&)> eval_fitness_;

    // null listener
    libevocosm::null_listener                       listener_;

public:
    /** @brief Constructor
        Creates a new function_optimizer with the given set of parameters.
        @param eval_fitness - the function to be optimized.
        @param a_init - a function that initializes solutions.
        @param init_population - a function that generates the initial population.
        @param a_population - The size of the solution population.
        @param a_mutation_rate - Mutation rate in the range [0,1].
        @param a_iterations - Number of iterations to perform when doing a run. */
    optimizer(boost::function<double(const solution_config&)> eval_fitness, boost::function<organism(void)> a_init,
              boost::function<std::vector<boost::shared_ptr<solution_config> >(void)> init_population,
              size_t a_population, double a_mutation_rate, size_t a_iterations);
    virtual ~optimizer() { delete evocosm_; }

    /** @brief Performs optimization
        This is where the work gets done; run iterates the number of times specificed when creating
        this function_optimizer, reporting via a function_reporter object on the progress of
        optimization.  */
    void run();

    /** @brief Organism factory
        Generates organisms for the initial populations in the evocosm. */
    virtual organism create();

    /** @brief Organism factory
        Appends several organisms to a population. */
    virtual void append(std::vector<organism> &population, size_t a_size);

    /** @brief Landscape factory
        Generates landscapes for an evocosm. I called this funtion "generate"
        to avoid collisions with the "create" method in organism_factory. */
    virtual landscape generate();
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace evolution
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // GA_EVOCOSM_H_INCLUDED
