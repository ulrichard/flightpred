// flightpred
#include "solution_config.h"
#include "lm_svm_dlib.h"
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
// boost
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <boost/version.hpp>
#if BOOST_VERSION >= 104000
  #include <boost/spirit/include/qi_core.hpp>
  #error will have to implement the parser with the new spirit
#else
  #include <boost/spirit/include/classic_core.hpp>
  #include <boost/spirit/include/classic_attribute.hpp>
  #include <boost/spirit/include/classic_symbols.hpp>
  #include <boost/spirit/include/phoenix1_actor.hpp>
  #include <boost/spirit/include/phoenix1_statements.hpp>
  using namespace boost::spirit::classic;
  using namespace phoenix;
#endif

using namespace flightpred;
using geometry::point_ll_deg;
using boost::shared_ptr;
using boost::bind;
using boost::ref;
using boost::lexical_cast;
using boost::array;
using std::string;
using std::vector;
using std::map;
using std::set;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
vector<shared_ptr<solution_config> > solution_config::get_initial_generation(const std::string &site_name)
{
    pqxx::transaction<> trans(flightpred_db::get_conn(), "initial generation");

    // get the id and geographic position of the prediction site
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

    // get the default feature descriptions of the weather data
    features_weather weather;
    const set<features_weather::feat_desc> features = weather.get_standard_features(pred_location);
    sstr.str("");
    std::copy(features.begin(), features.end(), std::ostream_iterator<features_weather::feat_desc>(sstr, " "));
    const string weather_feat_desc(sstr.str());

    vector<string> algo_desc;
    algo_desc.push_back("DLIB_RVM(RBF(0.01))");
    algo_desc.push_back("DLIB_RVM(RBF(0.007))");
    algo_desc.push_back("DLIB_RVM(RBF(0.005))");
    algo_desc.push_back("DLIB_RVM(RBF(0.001))");
    algo_desc.push_back("DLIB_RVM(RBF(0.0001))");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01)   0.01)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01)   0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01)   0.0001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01)   0.00001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.007)  0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.005)  0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.001)  0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.0001) 0.001)");


    vector<shared_ptr<solution_config> > solutions;
    BOOST_FOREACH(const string &desc, algo_desc)
        solutions.push_back(shared_ptr<solution_config>(new solution_config(site_name, desc + " " + weather_feat_desc)));


    return solutions;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
struct assignhour
{
    assignhour(boost::posix_time::time_duration &tdur) : tdur_(tdur) { }

    void operator()(const int val) const
    {
        tdur_ = boost::posix_time::hours(val);
    }
private:
    boost::posix_time::time_duration &tdur_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
struct assign_dlib_rvm_rbf
{
    assign_dlib_rvm_rbf(map<string, shared_ptr<learning_machine> > &learning_machines, double &gamma)
        : learning_machines_(learning_machines), gamma_(gamma) { }

    template<class iterT>
    void operator()(iterT begin, iterT end) const
    {
        typedef lm_dlib_rvm<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > krlsT;

        for(array<string, 5>::const_iterator it = flightpred_globals::pred_values.begin(); it != flightpred_globals::pred_values.end(); ++it)
            learning_machines_[*it] = shared_ptr<learning_machine>(new krlsT(*it, gamma_));
    }
private:
    map<string, shared_ptr<learning_machine> > &learning_machines_;
    double &gamma_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
struct assign_dlib_krls_rbf
{
    assign_dlib_krls_rbf(map<string, shared_ptr<learning_machine> > &learning_machines, double &gamma)
        : learning_machines_(learning_machines), gamma_(gamma) { }

    template<class iterT>
    void operator()(iterT begin, iterT end) const
    {
        typedef lm_dlib_krls<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > krlsT;

        for(array<string, 5>::const_iterator it = flightpred_globals::pred_values.begin(); it != flightpred_globals::pred_values.end(); ++it)
            learning_machines_[*it] = shared_ptr<learning_machine>(new krlsT(*it, gamma_));
    }
private:
    map<string, shared_ptr<learning_machine> > &learning_machines_;
    double &gamma_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void solution_config::decode()
{
    features_desc_.clear();

#if BOOST_VERSION >= 104000
  #error will have to implement the parser with the new spirit
#else

    features_weather::feat_desc currfeat;
    vector<features_weather::feat_desc> features;
    double currgamma, currfact;

    void (point_ll_deg::*setlon)(const double &v) = &point_ll_deg::lon;
    void (point_ll_deg::*setlat)(const double &v) = &point_ll_deg::lat;
	typedef rule<> rule_t;

	rule_t kernel_dlib_rbf = "RBF(" >> ureal_p[assign_a(currgamma)] >> ")";
	rule_t algo_dlib_rvm  = ("DLIB_RVM(" >> kernel_dlib_rbf >> ")")
            [assign_dlib_rvm_rbf(learning_machines_, currgamma)];
	rule_t algo_dlib_krls = ("DLIB_KRLS(" >> kernel_dlib_rbf >> *blank_p >> ureal_p[assign_a(currfact)] >> ")")
            [assign_dlib_krls_rbf(learning_machines_, currgamma)];
	rule_t algo = algo_dlib_rvm | algo_dlib_krls;

	rule_t model_gfs = str_p("GFS")[assign_a(currfeat.model)];
	rule_t model = model_gfs;
	rule_t reltime = int_p[assignhour(currfeat.reltime)] >> ":00:00";
	rule_t location = "POINT(" >> real_p[bind(setlon, ref(currfeat.location), _1)]
                    >> blank_p >> real_p[bind(setlat, ref(currfeat.location), _1)] >> ")";
	rule_t level = int_p[assign_a(currfeat.level)];
	rule_t param = (+upper_p)[assign_a(currfeat.param)];
	rule_t weather_feature = ("FEATURE(" >> model >> blank_p >> reltime >> blank_p >> location >> blank_p >> level >> blank_p >> param >> ")")[push_back_a(features, currfeat)];

	rule_t solution_description = algo >> blank_p >> weather_feature % blank_p;

    parse_info<> info = parse(solution_description_.c_str(), solution_description, space_p);
    if(!info.hit || info.length < solution_description_.length() - 1)
        throw std::runtime_error("failed to parse solution description : " + string(info.stop) + " " + lexical_cast<string>(features.size()));

    std::copy(features.begin(), features.end(), std::inserter(features_desc_, features_desc_.end()));
#endif
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
shared_ptr<learning_machine> solution_config::get_decision_function(const std::string &eval_name)
{
    std::map<std::string, boost::shared_ptr<learning_machine> >::iterator fit = learning_machines_.find(eval_name);
    if(fit == learning_machines_.end())
        throw std::invalid_argument("no decision function for " + eval_name);
    return fit->second;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A


