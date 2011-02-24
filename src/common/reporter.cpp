
// flightpred
#include "reporter.h"

using namespace flightpred::reporting;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
ReportDispatcher * ReportDispatcher::inst_ = 0;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
Level flightpred::reporting::ParseLevel(const std::string& lvlstr, const Level fallback)
{
    if(lvlstr == "DEBUGING")
        return DEBUGING;
    if(lvlstr == "VERBOSE")
        return VERBOSE;
    if(lvlstr == "INFO")
        return INFO;
    if(lvlstr == "WARN")
        return WARN;
    if(lvlstr == "ERROR")
        return ERROR;
    if(lvlstr == "SEVERE")
        return SEVERE;
    return fallback;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A


