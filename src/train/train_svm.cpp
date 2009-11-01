// flightpred
#include "train_svm.h"
#include "extract_features_flight.h"
#include "common/features_weather.h"
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// dlib
#include <dlib/svm.h>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
//#include <geometry/io/wkt/aswkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/timer.hpp>
#include <boost/array.hpp>
// standard library
#include <sstream>
#include <fstream>

using namespace flightpred;
namespace bgreg = boost::gregorian;
using boost::array;
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

    const size_t num_fl_lbl = 5;
    vector<sample_type>               samples;
    array<vector<double>, num_fl_lbl> labels;

    for(bgreg::date day = from; day <= to; day += bgreg::days(1))
    {
        const vector<double> valflights = flights.get_features(pred_site_id, day);
        assert(valflights.size() == num_fl_lbl);
        std::cout << "collecting features for " << bgreg::to_iso_extended_string(day)
                  << " "     << valflights[0] << " flights "
                  << " max " << valflights[1] << " km "
                  << " avg " << valflights[2] << " km" << std::endl;
        const vector<double> valweather = weather.get_features(features, day);
        assert(valweather.size() == features.size());
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

        for(size_t i=0; i<num_fl_lbl; ++i)
            labels[i].push_back(valflights[i]);
    }

    // normalize the samples
    std::cout << "normalize the samples" << std::endl;
    dlib::vector_normalizer<sample_type> normalizer;
    normalizer.train(samples);
    std::transform(samples.begin(), samples.end(), samples.begin(), normalizer);
//    for(unsigned long i = 0; i < samples.size(); ++i)
//        samples[i] = normalizer(samples[i]);
    size_t oid_normalizer = 0;
    if(true)
    {
        std::cout << "streaming the normalizer to the db blob" << std::endl;
        pqxx::largeobject dblobj(trans);
        pqxx::olostream dbstrm(trans, dblobj);
        serialize(normalizer, dbstrm);
        oid_normalizer = dblobj.id();
    }

    const array<string, num_fl_lbl> svm_names = {"svm_num_flight", "svm_max_dist", "svm_avg_dist", "svm_max_dur", "svm_avg_dur"};
    array<size_t, num_fl_lbl> blob_ids;
    double traintime = 0.0;
    for(size_t i=0; i<num_fl_lbl; ++i)
    {
        std::cout << "train the support vector machine for " << svm_names[i] << std::endl;
        boost::timer btim;
        dlib::decision_function<kernel_type> learnedfunc = trainer.train(samples, labels[i]);
        traintime += btim.elapsed();

        // serialize the svm to the database blob
        std::cout << "streaming the svm to the db blob" << std::endl;
        pqxx::largeobject dblobj(trans);
        pqxx::olostream dbstrm(trans, dblobj);
        dlib::serialize(learnedfunc, dbstrm);
        blob_ids[i] = dblobj.id();
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
    sstr << "INSERT INTO trained_solutions (pred_site_id, configuration, ";
    std::copy(svm_names.begin(), svm_names.end(), std::ostream_iterator<string>(sstr, ", "));
    sstr << "normalizer, score, train_time, generation) "
         << "VALUES (" << pred_site_id << ", 'SVM(RBF 0.05) ";
    std::copy(features.begin(), features.end(), std::ostream_iterator<features_weather::feat_desc>(sstr, " "));
    sstr << "', ";
    std::copy(blob_ids.begin(), blob_ids.end(), std::ostream_iterator<size_t>(sstr, ", "));
    sstr << oid_normalizer << ", " << score << ", " << traintime << ", " << generation << ")";
    res = trans.exec(sstr.str());

    trans.commit();
/*
    // at the moment additionally serialize to a file
    std::cout << "streaming the svm to a temp file" << std::endl;
    std::ofstream fout("/tmp/saved_function.dat", std::ios::binary);
    serialize(learnedfunc,fout);
    fout.close();
*/

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

