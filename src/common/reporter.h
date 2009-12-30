#ifndef REPORTER_H_INCLUDED
#define REPORTER_H_INCLUDED

// boost
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/filesystem/fstream.hpp>
// std lib
#include <string>
#include <iostream>
#include <sstream>
#include <utility>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
namespace reporting
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
enum Level
{
    DEBUGING,
    VERBOSE,
    INFO,
    WARN,
    ERROR,
    SEVERE
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief abstract interface */
class ListenerBase
{
public:
    virtual void msg(const std::string &txt) = 0;
    virtual ~ListenerBase() { }
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief leaking singleton */
class ReportDispatcher : public boost::noncopyable
{
public:
    static ReportDispatcher & inst()
    {
        if(!inst_)
            inst_ = new ReportDispatcher();
        return *inst_;
    }

    void add(ListenerBase *lst, Level lvl)
    {
        listeners_.insert(lvl, lst);
    }

    void msg(const std::string &txt, Level lvl)
    {
        boost::ptr_multimap<Level, ListenerBase>::iterator ite = listeners_.upper_bound(lvl), it = listeners_.begin();
        for( ; it != ite; ++it)
            it->second->msg(txt);
    }

private:
    ReportDispatcher() { }
    boost::ptr_multimap<Level, ListenerBase> listeners_;
    static ReportDispatcher * inst_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief for convenience */
class report : public boost::noncopyable
{
public:
    explicit report(Level lvl) : lvl_(lvl) {}
    ~ report() { ReportDispatcher::inst().msg(sstr_.str(), lvl_); }

    template<class InputT>
    std::stringstream & operator<<(InputT &inp)
    {
        sstr_ << inp;
        return sstr_;
    }

    void msg(const std::string &txt)
    {
        sstr_ << txt;
    }


private:
    Level lvl_;
    std::stringstream sstr_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/** @brief abstract interface */
class ListenerCout : public ListenerBase
{
public:
    virtual void msg(const std::string &txt)
    {
        std::cout << txt << std::endl;
    }
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class ListenerFile : public ListenerBase
{
public:
    ListenerFile(const boost::filesystem::path &errfile) : ofs_(errfile) { }

    virtual void msg(const std::string &txt)
    {
        ofs_ << txt << std::endl;
    }
private:
    boost::filesystem::ofstream ofs_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace
} // namespace
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // REPORTER_H_INCLUDED
