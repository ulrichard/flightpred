#ifndef GA_EVOCOSM_H_INCLUDED
#define GA_EVOCOSM_H_INCLUDED

// flightpred
#include "solution_config.h"
// evocosm
#include <libevocosm/listener.h>
#include <libevocosm/organism.h>
#include <libevocosm/mutator.h>
#include <libevocosm/landscape.h>
//#include <libevocosm/reporter.h>
#include <libevocosm/reproducer.h>
#include <libevocosm/evocosm.h>
#include <libevocosm/evoreal.h>
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

    /** organisms with less error come before ones with bigger error. */
    virtual bool operator<(const libevocosm::organism<solution_config> &rhs) const
    {
        return (fitness < rhs.fitness);
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

    virtual void mutate(std::vector<organism> &population) { };

private:
    double mutation_rate_;
    static libevocosm::evoreal evoreal_; //! Provides mutation and crossover services for doubles
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class reproducer : public libevocosm::reproducer<organism>
{
public:
    reproducer(const size_t pred_site_id, const double mutation_rate, size_t &current_generation);
    reproducer(const reproducer &src)
        : pred_site_id_(src.pred_site_id_), site_location_(src.site_location_), mutation_rate_(src.mutation_rate_), current_generation_(src.current_generation_), solution_descriptions_(src.solution_descriptions_) { }
    virtual ~reproducer()  { }
    reproducer & operator=(const reproducer &source)
    {
        pred_site_id_  = source.pred_site_id_;
        site_location_ = source.site_location_;
        mutation_rate_ = source.mutation_rate_;
        current_generation_ = source.current_generation_;
        solution_descriptions_ = source.solution_descriptions_;
        return *this;
    }

    /** @brief Reproduction for solutions
        Breeds new solutions, by cloning or the combination of elements from parent organisms.
        In the flightprediction system we don't use sexual reproduction, meaning crossover, but only cloning with mutations.
        \param a_population - A population of solutions
        \param p_limit - Maximum number of children
        \return A vector containing new "child" chromosomes */
    virtual std::vector<organism> breed(const std::vector<organism> &old_population, size_t p_limit);

private:
    solution_config make_mutated_clone(const solution_config &src);

    size_t pred_site_id_;
    boost::geometry::model::ll::point<> site_location_;
    double mutation_rate_;
    size_t &current_generation_;
    std::set<std::string> solution_descriptions_; //! all the descriptions in the system so far.
    static libevocosm::evoreal evoreal_; //! Provides mutation and crossover services for doubles
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief Defines the test for a population of solutions
    A "fitness" landscape defines the environment in which an organism
    competes for survival.*/
class landscape : public libevocosm::landscape<organism>
{
public:
    landscape(boost::function<double(const solution_config&)> eval_fitness, libevocosm::listener<organism> &lstnr)
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
        orgn.fitness = eval_fitness_(orgn.genes);
        return orgn.fitness;
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
/*
class reporter : public libevocosm::reporter<organism, landscape>
{
public:
    reporter() : libevocosm::reporter<organism, landscape>(listener_) { }

    /// @brief say something about a population
    virtual bool report(const std::vector<std::vector<organism> > &population, size_t iteration, double &fitness, bool finished = false);

protected:
    libevocosm::listener_stdout listener_;

};
*/
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief A generic optimizer
    Using instances of the other classes, this class binds together the pieces to
    create a complete optimizer. A user of this class defines two functions
    -- a solution initializer and a fitness test -- that define the target problem.*/
class optimizer //: protected libevocosm::organism_factory<organism>, protected libevocosm::landscape_factory<landscape>
{
public:
    /** @brief Constructor
        Creates a new function_optimizer with the given set of parameters.
        @param eval_fitness - the function to be optimized.
        @param a_init - a function that initializes solutions.
        @param init_population - a function that generates the initial population.
        @param a_population - The size of the solution population.
        @param a_mutation_rate - Mutation rate in the range [0,1].
        @param a_iterations - Number of iterations to perform when doing a run.
        @param pred_site_id - the database id of the flying site we optimize the predictions. */
    optimizer(boost::function<double(const solution_config&)> eval_fitness, boost::function<organism(void)> a_init,
              boost::function<std::vector<boost::shared_ptr<solution_config> >(void)> init_population,
              size_t population, double mutation_rate, size_t iterations, size_t pred_site_id);
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

private:
    size_t current_generation_;

    // objects that define the characteristics of the genetic algorithm
    mutator                                mutator_;
    reproducer                             reproducer_;
    libevocosm::null_scaler<organism>      scaler_;
//    libevocosm::null_migrator<organism>    migrator_;
    libevocosm::elitism_selector<organism> selector_;
//    reporter                               reporter_;

    libevocosm::null_listener<organism>     listener_;     // null listener -> does nothing at the event points
    std::vector<organism>                   population_;
    landscape                               landscape_;
    libevocosm::analyzer<organism>          analyzer_;

    libevocosm::evocosm<organism>*          evocosm_; // the evocosm binds it all together;
    const size_t iterations_; // number of iterations to run
    boost::function<organism(void)> init_;  // a function that provides initialized, random solutions
    boost::function<std::vector<boost::shared_ptr<solution_config> >(void)> init_population_;  // a function that generates the initial population
    boost::function<double(const solution_config&)> eval_fitness_;  // the function to be optimized
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace evolution
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // GA_EVOCOSM_H_INCLUDED

