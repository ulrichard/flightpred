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
#include <boost/regex.hpp>
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
solution_config::solution_config(const size_t db_id)
{
    train_sol_id_ = db_id;
    pqxx::transaction<> trans(flightpred_db::get_conn(), "load solution config");

    std::stringstream sstr;
    sstr << "SELECT configuration, site_name FROM trained_solutions INNER JOIN pred_sites "
         << "ON trained_solutions.pred_site_id = pred_sites.pred_site_id "
         << "WHERE train_sol_id=" << train_sol_id_;
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("no trained_solution found for  train_sol_id=" + db_id);

    res[0]["configuration"].to(solution_description_);
    res[0]["site_name"].to(site_name_);
    trans.commit();

    decode();

    assert(features_desc_.size() > 40);
    assert(learning_machines_.size() >= 1);

    // de-serialize the svm's from the database blob
    for(std::map<std::string, boost::shared_ptr<learning_machine> >::iterator it = learning_machines_.begin(); it != learning_machines_.end(); ++it)
        it->second->read_from_db(train_sol_id_);
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
        typedef dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > kernT;
        typedef lm_dlib_rvm<kernT> rvmT;

        kernT kern(gamma_);
        for(array<string, 5>::const_iterator it = flightpred_globals::pred_values.begin(); it != flightpred_globals::pred_values.end(); ++it)
            learning_machines_[*it] = shared_ptr<learning_machine>(new rvmT(*it, kern));
    }
private:
    map<string, shared_ptr<learning_machine> > &learning_machines_;
    double &gamma_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
struct assign_dlib_rvm_sig
{
    assign_dlib_rvm_sig(map<string, shared_ptr<learning_machine> > &learning_machines, double &gamma, double &coef)
        : learning_machines_(learning_machines), gamma_(gamma), coef_(coef) { }

    template<class iterT>
    void operator()(iterT begin, iterT end) const
    {
        typedef dlib::sigmoid_kernel<dlib::matrix<double, 0, 1> > kernT;
        typedef lm_dlib_rvm<kernT> rvmT;

        kernT kern(gamma_, coef_);
        for(array<string, 5>::const_iterator it = flightpred_globals::pred_values.begin(); it != flightpred_globals::pred_values.end(); ++it)
            learning_machines_[*it] = shared_ptr<learning_machine>(new rvmT(*it, kern));
    }
private:
    map<string, shared_ptr<learning_machine> > &learning_machines_;
    double &gamma_;
    double &coef_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
struct assign_dlib_rvm_poly
{
    assign_dlib_rvm_poly(map<string, shared_ptr<learning_machine> > &learning_machines, double &gamma, double &coef, double &degree)
        : learning_machines_(learning_machines), gamma_(gamma), coef_(coef), deg_(degree) { }

    template<class iterT>
    void operator()(iterT begin, iterT end) const
    {
        typedef dlib::polynomial_kernel<dlib::matrix<double, 0, 1> > kernT;
        typedef lm_dlib_rvm<kernT> rvmT;

        kernT kern(gamma_, coef_, deg_);
        for(array<string, 5>::const_iterator it = flightpred_globals::pred_values.begin(); it != flightpred_globals::pred_values.end(); ++it)
            learning_machines_[*it] = shared_ptr<learning_machine>(new rvmT(*it, kern));
    }
private:
    map<string, shared_ptr<learning_machine> > &learning_machines_;
    double &gamma_;
    double &coef_;
    double &deg_;
};/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
struct assign_dlib_krls_rbf
{
    assign_dlib_krls_rbf(map<string, shared_ptr<learning_machine> > &learning_machines, double &gamma, double &fact)
        : learning_machines_(learning_machines), gamma_(gamma), fact_(fact) { }

    template<class iterT>
    void operator()(iterT begin, iterT end) const
    {
        typedef dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > kernT;
        typedef lm_dlib_krls<kernT> krlsT;

        assert(5 == flightpred_globals::pred_values.size());
        kernT kern(gamma_);
        for(array<string, 5>::const_iterator it = flightpred_globals::pred_values.begin(); it != flightpred_globals::pred_values.end(); ++it)
            learning_machines_[*it] = shared_ptr<learning_machine>(new krlsT(*it, kern, fact_));
    }
private:
    map<string, shared_ptr<learning_machine> > &learning_machines_;
    double &gamma_;
    double &fact_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const std::string solution_config::get_short_description() const
{
    boost::regex regx("\\w+\\(\\w+\\((\\d+\\.\\d+\\s*){1,3}\\)(\\s*\\d+\\.\\d+)?\\s*\\)");
    boost::smatch regxmatch;
    if(!boost::regex_search(solution_description_, regxmatch, regx))
        throw std::invalid_argument("failed to extract short description from : " + solution_description_);

    std::stringstream sstr;
    sstr << regxmatch[0] << " with " << features_desc_.size() << " features";
    return sstr.str();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void solution_config::decode()
{
    features_desc_.clear();

#if BOOST_VERSION >= 104000
  #error will have to implement the parser with the new spirit
#else

    features_weather::feat_desc currfeat;
    vector<features_weather::feat_desc> features;
    double currgamma, currcoef, currdegree, currfact;

    void (point_ll_deg::*setlon)(const double &v) = &point_ll_deg::lon;
    void (point_ll_deg::*setlat)(const double &v) = &point_ll_deg::lat;
	typedef rule<> rule_t;

	rule_t kernel_dlib_rbf  = "RBF(" >> ureal_p[assign_a(currgamma)] >> ")";
	rule_t kernel_dlib_sig  = "SIG(" >> ureal_p[assign_a(currgamma)] >> *blank_p >> ureal_p[assign_a(currcoef)] >> ")";
	rule_t kernel_dlib_poly = "POLY(" >> ureal_p[assign_a(currgamma)] >> *blank_p >> ureal_p[assign_a(currcoef)] >> *blank_p >> ureal_p[assign_a(currdegree)] >> ")";

	rule_t algo_dlib_rvm_rbf  = ("DLIB_RVM(" >> kernel_dlib_rbf >> ")")
            [assign_dlib_rvm_rbf(learning_machines_, currgamma)];
	rule_t algo_dlib_rvm_sig  = ("DLIB_RVM(" >> kernel_dlib_sig >> ")")
            [assign_dlib_rvm_sig(learning_machines_, currgamma, currcoef)];
	rule_t algo_dlib_rvm_poly  = ("DLIB_RVM(" >> kernel_dlib_poly >> ")")
            [assign_dlib_rvm_poly(learning_machines_, currgamma, currcoef, currdegree)];
	rule_t algo_dlib_krls = ("DLIB_KRLS(" >> kernel_dlib_rbf >> *blank_p >> ureal_p[assign_a(currfact)] >> ")")
            [assign_dlib_krls_rbf(learning_machines_, currgamma, currfact)];
	rule_t algo = algo_dlib_rvm_rbf | algo_dlib_rvm_sig | algo_dlib_rvm_poly | algo_dlib_krls;

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
shared_ptr<learning_machine> solution_config::get_decision_function(const std::string &eval_name) const
{
    std::map<std::string, boost::shared_ptr<learning_machine> >::const_iterator fit = learning_machines_.find(eval_name);
    if(fit == learning_machines_.end())
        throw std::invalid_argument("no decision function for " + eval_name);
    return fit->second;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
