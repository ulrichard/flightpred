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
#include <boost/version.hpp>
#if BOOST_VERSION >= 104000
  #include <boost/spirit/include/qi_core.hpp>
  #error will have to implement the parser with the new spirit
#else
  #include <boost/spirit/include/classic_core.hpp>
  #include <boost/spirit/include/classic_attribute.hpp>
  #include <boost/spirit/include/classic_symbols.hpp>
//  #include <boost/spirit/include/classic_chset.hpp>
  using namespace boost::spirit::classic;
#endif

using namespace flightpred;
using boost::shared_ptr;
using std::string;
using std::vector;
using std::map;
using std::set;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
vector<shared_ptr<solution_config> > solution_config::get_initial_generation(const std::string &site_name, const std::string &db_conn_str)
{
    pqxx::connection conn(db_conn_str);
    pqxx::transaction<> trans(conn, "initial generation");

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
    features_weather weather(db_conn_str);
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
    algo_desc.push_back("DLIB_KRLS(RBF(0.01),   0.01)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01),   0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01),   0.0001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.01),   0.00001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.007),  0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.005),  0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.001),  0.001)");
    algo_desc.push_back("DLIB_KRLS(RBF(0.0001), 0.001)");


    vector<shared_ptr<solution_config> > solutions;
    BOOST_FOREACH(const string &desc, algo_desc)
        solutions.push_back(shared_ptr<solution_config>(new solution_config(site_name, db_conn_str, desc + " " + weather_feat_desc)));


    return solutions;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void solution_config::decode()
{
#if BOOST_VERSION >= 104000
  #error will have to implement the parser with the new spirit
#else

//    typedef char                    char_t;
//	typedef file_iterator <char_t>  iterator_t;
//	typedef scanner<iterator_t>     scanner_t;
//	typedef rule<scanner_t>         rule_t;
	typedef rule<>         rule_t;

	rule_t kernel_dlib_rbf = "RBF(" >> ureal_p >> ")";
	rule_t kernel_dlib = kernel_dlib_rbf;

	rule_t algo_dlib_rvm  = "DLIB_RVM(" >> kernel_dlib >> ")";
	rule_t algo_dlib_krls = "DLIB_KRLS(" >> kernel_dlib >> ")";
	rule_t algo = algo_dlib_rvm | algo_dlib_krls;

	rule_t model_gfs = str_p("GFS");
	rule_t model = model_gfs;

	rule_t reltime = int_p >> ":00:00";

	rule_t location = "POINT(" >> real_p >> real_p >> ")";

	rule_t level = int_p;

	rule_t param = +upper_p;

	rule_t weather_feature = "FEATURE(" >> model >> reltime >> location >> level >> param >> ")";

	rule_t solution_description = algo >> weather_feature % ' ';

    parse_info<> info = parse(solution_description_.c_str(), solution_description, space_p);
    if(!info.full)
        throw std::runtime_error("failed to parse solution description");
#endif
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A



