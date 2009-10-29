// flightpred
#include "train_svm.h"
#include "extract_features_flight.h"
#include "features_weather.h"
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// dlib
#include "dlib/svm.h"
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
//#include <geometry/io/wkt/aswkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/timer.hpp>
// standard library
#include <sstream>

using namespace flightpred;
namespace bgreg = boost::gregorian;
using std::vector;
using std::set;
using std::string;
using std::cout;
using std::endl;

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

    // set up the dlib svm
    typedef dlib::matrix<double, 0, 1> sample_type;
    typedef dlib::radial_basis_kernel<sample_type> kernel_type;
    dlib::rvm_regression_trainer<kernel_type> trainer;
    trainer.set_kernel(kernel_type(0.05));

    vector<sample_type> samples;
    vector<double>      labels;


    for(bgreg::date day = from; day <= to; day += bgreg::days(1))
    {
        std::cout << "collecting features for " << bgreg::to_iso_extended_string(day) << std::endl;
        const vector<double> valflights = flights.get_features(pred_site_id, day);
        const vector<double> valweather = weather.get_features(features, day);
        // put together the values to feet to the svm
        vector<double> featval;
        featval.push_back(day.year());
        featval.push_back(day.day_of_year());
        copy(valweather.begin(), valweather.end(), std::back_inserter(featval));

        // feed the svm
        sample_type samp;
        samp.set_size(featval.size());
        for(size_t i=0; i<featval.size(); ++i)
            samp(i) = featval[i];
        samples.push_back(samp);

        labels.push_back(valflights[0]);

    }

    boost::timer btim;
    // normalize the samples
    dlib::vector_normalizer<sample_type> normalizer;
    normalizer.train(samples);
    for (unsigned long i = 0; i < samples.size(); ++i)
        samples[i] = normalizer(samples[i]);

    dlib::decision_function<kernel_type> learnedfunc = trainer.train(samples, labels);

    // rate the solution
    const double traintime = btim.elapsed();
    const double score = 0.0;

    // serialize the svm to the database blob
    pqxx::largeobject dblobj(trans);
    pqxx::olostream dbstrm(trans, dblobj);
    dlib::serialize(learnedfunc, dbstrm);

    // register the solution in the db
    sstr.str("");
    sstr << "INSERT INTO trained_solutions (pred_site_id, configuration, svm_ser, score, train_time, generation) "
         << "VALUES (" << pred_site_id << ", 'SVM(RBF 0.05) ";
    std::copy(features.begin(), features.end(), std::ostream_iterator<features_weather::feat_desc>(sstr, " "));
    sstr << "', " << dblobj.id() << ", " << score << ", " << traintime << ", 0)";
    res = trans.exec(sstr.str());


    trans.commit();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

