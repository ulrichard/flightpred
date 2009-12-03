// flightpred
#include "forecast.h"
#include "common/lm_svm_dlib.h"
#include "common/flightpred_globals.h"
// postgre
#include <pqxx/pqxx>
//#include <pqxx/largeobject>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/foreach.hpp>
// standard library
#include <sstream>
#include <fstream>
#include <algorithm>
#include <utility>
#include <limits>

using namespace flightpred;
namespace bgreg = boost::gregorian;
using boost::array;
using std::vector;
using std::string;
using std::map;
using std::pair;
using std::numeric_limits;
//using std::cout;
//using std::endl;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
map<string, double> forecast::predict(const string &site_name, const bgreg::date &pred_day)
{
    if(site_name != site_name_)
    {
        load_learned_functins(site_name);
        site_name_ = site_name;
    }

    vector<double> samp;
    load_features(pred_day, std::inserter(samp, samp.end()));

    // feed samples to the learned functions
    map<string, double> predval;
    for(size_t k=0; k<flightpred_globals::pred_values.size(); ++k)
    {
        double val = learnedfunctions_[k].eval(samp);
        if(isnan(val) || val < 0.0)
           val = 0.0;
        predval["svm_" + flightpred_globals::pred_values[k]] = val;
    }

    return predval;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void forecast::load_learned_functins(const std::string &site_name)
{
    pqxx::transaction<> trans(flightpred_db::get_conn(), "flight forecast");

    std::stringstream sstr;
    sstr << "SELECT pred_site_id, AsText(location) as loc FROM pred_sites where site_name='" << site_name << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::invalid_argument("site not found: " + site_name);

    size_t tmp_pred_site_id;
    res[0]["pred_site_id"].to(tmp_pred_site_id);
    const size_t pred_site_id = tmp_pred_site_id;
    string tmpstr;
    res[0]["loc"].to(tmpstr);
    if(!geometry::from_wkt(tmpstr, pred_location_))
        throw std::runtime_error("failed to parse the prediction site location as retured from the database : " + tmpstr);

    std::cout << "collecting features and running SVM for " << site_name << std::endl;
    sstr.str("");
    sstr << "SELECT train_sol_id, configuration, score, train_time, generation FROM trained_solutions WHERE "
         << "pred_site_id=" << pred_site_id << " ORDER BY score DESC, generation DESC";
    res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("no configuration found for : " + site_name);

    res[0]["train_sol_id"].to(solution_id_);
    res[0]["configuration"].to(feature_desc_);
    trans.commit();

    // de-serialize the svm's from the database blob
    for(size_t j=0; j<flightpred_globals::pred_values.size(); ++j)
    {
//        typedef lm_dlib_rvm<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > dlibtrainer;
        typedef lm_dlib_krls<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > dlibtrainer;
        learnedfunctions_.push_back(new dlibtrainer("svm_" + flightpred_globals::pred_values[j], 0.01));
        learnedfunctions_.back().read_from_db(solution_id_);
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void forecast::prediction_run(const size_t pred_days)
{
    pqxx::transaction<> trans(flightpred_db::get_conn(), "flight prediction");
    const bgreg::date today(boost::posix_time::second_clock::universal_time().date());

    std::stringstream sstr;
    sstr << "SELECT pred_site_id, site_name FROM pred_sites";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::invalid_argument("no sites found");
    vector<pair<size_t, string> > sites;

    for(size_t l=0; l<res.size(); ++l)
    {
        size_t pred_site_id;
        res[l]["pred_site_id"].to(pred_site_id);
        string site_name;
        res[l]["site_name"].to(site_name);
        sites.push_back(std::make_pair(pred_site_id, site_name));
    }

    for(vector<pair<size_t, string> >::const_iterator it = sites.begin(); it != sites.end(); ++it)
    {
        for(size_t j=0; j<pred_days; ++j)
        {
            const bgreg::date day(today + bgreg::days(j));

            // let the machine make it's (educated) guesses
            map<string, double> predres = predict(it->second, day);

            sstr.str("");
            sstr << "INSERT INTO flight_pred (pred_site_id, train_sol_id,";
            std::copy(flightpred_globals::pred_values.begin(), flightpred_globals::pred_values.end(), std::ostream_iterator<string>(sstr, ", "));
            sstr << "calculated, pred_day) VALUES (" << it->first << ", " << solution_id_ << ", ";

            BOOST_FOREACH(string predname, flightpred_globals::pred_values)
            {
                double val = predres["svm_" + predname];
                sstr << val << ", ";
                std::cout << it->second << " " << bgreg::to_iso_extended_string(day) << " "
                          << predname << " " << val << std::endl;

            }

            sstr << "Now(), '" <<  bgreg::to_iso_extended_string(day) << "')";
//            std::cout << sstr.str() << std::endl;
            res = trans.exec(sstr.str());
        }
    }
    trans.commit();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

