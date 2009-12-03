// flightpred
#include "flightpred_globals.h"

using namespace flightpred;


flightpred_db *flightpred_db::inst_ = 0;

const boost::array<std::string, 5> flightpred_globals::pred_values = {"num_flight", "max_dist", "avg_dist", "max_dur", "avg_dur"};

