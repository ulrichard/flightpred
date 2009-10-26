// flightpred
#include "train_svm.h"
#include "extract_features_flight.h"
#include "features_weather.h"
// postgre
#include <pqxx/pqxx>
// dlib
#include "dlib/svm.h"
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
//#include <geometry/io/wkt/aswkt.hpp>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
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

    // normalize the samples
    dlib::vector_normalizer<sample_type> normalizer;
    normalizer.train(samples);
    for (unsigned long i = 0; i < samples.size(); ++i)
        samples[i] = normalizer(samples[i]);

    dlib::randomize_samples(samples, labels);

    const double max_nu = dlib::maximum_nu(labels);

    dlib::svm_nu_trainer<kernel_type> trainer;

    // Now we loop over some different nu and gamma values to see how good they are.  Note
    // that this is a very simple way to try out a few possible parameter choices.  You
    // should look at the model_selection_ex.cpp program for examples of more sophisticated
    // strategies for determining good parameter choices.
    cout << "doing cross validation" << endl;
    for(double gamma = 0.00001; gamma <= 1; gamma += 0.1)
    {
//        for(double nu = 0.00001; nu < max_nu; nu += 0.1)
        double nu = 0.0;
        {
            // tell the trainer the parameters we want to use
            trainer.set_kernel(kernel_type(gamma));
            trainer.set_nu(nu);

            cout << "gamma: " << gamma << "    nu: " << nu;
            // Print out the cross validation accuracy for 3-fold cross validation using the current gamma and nu.
            // cross_validate_trainer() returns a row vector.  The first element of the vector is the fraction
            // of +1 training examples correctly classified and the second number is the fraction of -1 training
            // examples correctly classified.
            cout << "     cross validation accuracy: " << cross_validate_trainer(trainer, samples, labels, 3);
        }
    }


    // Now we train on the full set of data and obtain the resulting decision function.  We use the
    // value of 0.1 for nu and gamma.  The decision function will return values >= 0 for samples it predicts
    // are in the +1 class and numbers < 0 for samples it predicts to be in the -1 class.
    trainer.set_kernel(kernel_type(0.1));
    trainer.set_nu(0.0);
    typedef dlib::decision_function<kernel_type> dec_funct_type;
    typedef dlib::normalized_function<dec_funct_type> funct_type;

    // Here we are making an instance of the normalized_function object.  This object provides a convenient
    // way to store the vector normalization information along with the decision function we are
    // going to learn.
    funct_type learned_function;
    learned_function.normalizer = normalizer;  // save normalization information
    learned_function.function = trainer.train(samples, labels); // perform the actual SVM training and save the results

    // print out the number of support vectors in the resulting decision function
    cout << "\nnumber of support vectors in our learned_function is "
         << learned_function.function.support_vectors.nr() << endl;


    trans.commit();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

