// flightpred
#include "train_svm.h"
#include "extract_features_flight.h"
#include "common/features_weather.h"
#include "common/lm_svm_dlib.h"
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
//#include <geometry/io/wkt/aswkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/timer.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
// standard library
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace flightpred;
namespace bgreg = boost::gregorian;
using boost::array;
using std::vector;
using std::set;
using std::pair;
using std::string;
using std::cout;
using std::endl;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void train_svm::train_all(const bgreg::date &from, const bgreg::date &to)
{
    vector<string> sites;

    {
        pqxx::connection conn(db_conn_str_);
        pqxx::transaction<> trans(conn, "collect features");

        // get the id of the prediction site
        std::stringstream sstr;
        sstr << "SELECT site_name FROM pred_sites";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            throw std::invalid_argument("no sites found");
        string tmpstr;
        for(int i=0; i<res.size(); ++i)
        {
            res[0][0].to(tmpstr);
            sites.push_back(tmpstr);
        }
    }

    std::for_each(sites.begin(), sites.end(), boost::bind(&train_svm::train, this, _1, from, to));
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void train_svm::train(const string &site_name, const bgreg::date &from, const bgreg::date &to)
{
    feature_extractor_flight flights(db_conn_str_);
    features_weather         weather(db_conn_str_);

    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "collect features");

    // get the id of the prediction site
    std::stringstream sstr;
    sstr << "SELECT pred_site_id, AsText(location) as loc FROM pred_sites WHERE site_name='" << site_name << "'";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::invalid_argument("site not found : " + site_name);
    size_t tmp_pred_site_id;
    res[0]["pred_site_id"].to(tmp_pred_site_id);
    const size_t pred_site_id = tmp_pred_site_id;
    string tmpstr;
    res[0]["loc"].to(tmpstr);
    geometry::point_ll_deg pred_location;
    if(!geometry::from_wkt(tmpstr, pred_location))
        throw std::runtime_error("failed to parse the prediction site location as retured from the database : " + tmpstr);

    // get the feature descriptions of the weather data
//    const set<features_weather::feat_desc> features = weather.decode_feature_desc("here comes the description");
    const set<features_weather::feat_desc> features = weather.get_standard_features(pred_location);

    const size_t num_fl_lbl = 5;
    learning_machine::SampleType      training_samples;
    array<vector<double>, num_fl_lbl> labels;

    for(bgreg::date day = from; day <= to; day += bgreg::days(1))
    {
        const vector<double> valflights = flights.get_features(pred_site_id, day);
        assert(valflights.size() == num_fl_lbl);
        for(size_t i=0; i<num_fl_lbl; ++i)
            labels[i].push_back(valflights[i]);

        std::cout << "collecting features for " << bgreg::to_iso_extended_string(day)
                  << " "     << valflights[0] << " flights "
                  << " max " << valflights[1] << " km "
                  << " avg " << valflights[2] << " km" << std::endl;
        const vector<double> valweather = weather.get_features(features, day);
        assert(valweather.size() == features.size());

        // put together the values to feed to the svm
        training_samples.push_back(vector<double>());
        training_samples.back().push_back(day.year());
        training_samples.back().push_back(day.day_of_year());
        std::copy(valweather.begin(), valweather.end(), std::back_inserter(training_samples.back()));
    }

    const size_t generation = 0;
    const double score = 0.0;
    std::cout << "registering the config in the db" << std::endl;
    // delete previous default configuratinos
    if(generation == 0)
    {
        sstr.str("");
        sstr << "DELETE FROM trained_solutions WHERE configuration='SVM(RBF 0.05) ";
        std::copy(features.begin(), features.end(), std::ostream_iterator<features_weather::feat_desc>(sstr, " "));
        sstr << "' AND generation=0";
        res = trans.exec(sstr.str());
    }
    // register the solution in the db
    sstr.str("");
    sstr << "INSERT INTO trained_solutions (pred_site_id, configuration, generation) "
         << "VALUES (" << pred_site_id << ", 'SVM(RBF 0.05) ";
    std::copy(features.begin(), features.end(), std::ostream_iterator<features_weather::feat_desc>(sstr, " "));
    sstr << "', " << generation << ")";
    res = trans.exec(sstr.str());
    // get the id
    sstr.str("");
    sstr << "SELECT train_sol_id FROM trained_solutions WHERE configuration='SVM(RBF 0.05) ";
    std::copy(features.begin(), features.end(), std::ostream_iterator<features_weather::feat_desc>(sstr, " "));
        sstr << "' AND generation=" << generation;
    res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("newly inserted record not found");
    size_t conf_id;
    res[0][0].to(conf_id);
    trans.commit();

    const array<string, num_fl_lbl> svm_names = {"svm_num_flight", "svm_max_dist", "svm_avg_dist", "svm_max_dur", "svm_avg_dur"};
    array<size_t, num_fl_lbl> blob_ids;
    double traintime = 0.0;
    for(size_t i=0; i<num_fl_lbl; ++i)
    {
        std::cout << "train the support vector machine for " << svm_names[i] << std::endl;
        boost::timer btim;
        lm_svm_dlib dlibsvmtrainer(svm_names[i], db_conn_str_);
        dlibsvmtrainer.train(training_samples, labels[i]);
        traintime += btim.elapsed();
        dlibsvmtrainer.write_to_db(conf_id);
    }

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

