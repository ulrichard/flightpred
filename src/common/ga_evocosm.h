#ifndef GA_EVOCOSM_H_INCLUDED
#define GA_EVOCOSM_H_INCLUDED

// flightpred
#include "common/solution_config.h"
// evocosm
#include <libevocosm/organism.h>
#include <libevocosm/mutator.h>
#include <libevocosm/landscape.h>
#include <libevocosm/reporter.h>
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

    /** organisms with higher finess come before ones with lower fitness. */
    virtual bool operator<(const libevocosm::organism<solution_config> &rhs) const
    {
        return (m_fitness > rhs.fitness());
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

    virtual void mutate(std::vector<organism> &population) { assert(!"todo : implement"); };

private:
    double mutation_rate_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
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
//    virtual double test(std::vector<organism> &population, bool verbose = false) const { }
private:
    boost::function<double(const solution_config&)> eval_fitness_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class reporter : public libevocosm::reporter<organism, landscape>
{
public:
    reporter() : libevocosm::reporter<organism, landscape>(listener_) { }

    /** @brief say something about a population */
    virtual bool report(const std::vector<std::vector<organism> > population, size_t iteration, double &fitness, bool finished = false) { }

protected:
    libevocosm::listener_stdout listener_;

};/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace evolution
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // GA_EVOCOSM_H_INCLUDED

