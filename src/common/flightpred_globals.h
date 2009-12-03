#ifndef FLIGHTPRED_GLOBALS_H_INCLUDED
#define FLIGHTPRED_GLOBALS_H_INCLUDED

// postgre
#include <pqxx/pqxx>
// boost
#include <boost/array.hpp>
// std lib
#include <string>
#include <cassert>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class flightpred_db
{
protected:
    flightpred_db(const std::string &dbconnstr) : conn_(dbconnstr) { }
    ~flightpred_db() { }

public:
    static void init(const std::string &dbconnstr)
    {   if(inst_) throw std::runtime_error("trying to create second instance of flightpred_db"); inst_ = new flightpred_db(dbconnstr); }

//    static flightpred_db & get_inst()
//    {   assert(inst_); return *inst_;    }

    static pqxx::connection & get_conn()
    {   assert(inst_); return inst_->conn_;    }

private:
    static flightpred_db *inst_;
    pqxx::connection conn_;

};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class flightpred_globals
{
public:
    static const boost::array<std::string, 5> pred_values;

};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // FLIGHTPRED_GLOBALS_H_INCLUDED
