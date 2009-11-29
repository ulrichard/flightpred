// flightpred
#include "common/solution_manager.h"
#include "common/features_weather.h"
#include "common/extract_features_flight.h"
// postgre
#include <pqxx/pqxx>
#include <pqxx/largeobject>
// ggl (boost sandbox)
#include <geometry/io/wkt/fromwkt.hpp>
// boost
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace flightpred;
using geometry::point_ll_deg;
using boost::lexical_cast;
using boost::shared_ptr;
namespace bgreg = boost::gregorian;
using std::string;
using std::vector;
using std::map;
using std::set;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void solution_manager::initialize_populations()
{
    vector<string> sites;

    {
        pqxx::connection conn(db_conn_str_);
        pqxx::transaction<> trans(conn, "initialize populations");

        // get the id of the prediction site
        std::stringstream sstr;
        sstr << "SELECT site_name FROM pred_sites";
        pqxx::result res = trans.exec(sstr.str());
        if(!res.size())
            throw std::invalid_argument("no sites found");
        for(size_t i=0; i<res.size(); ++i)
        {
            string tmpstr;
            res[i][0].to(tmpstr);
            sites.push_back(tmpstr);
        }
    }

    std::for_each(sites.begin(), sites.end(), boost::bind(&solution_manager::initialize_population, this, _1));
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void solution_manager::initialize_population(const std::string &site_name)
{
    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "initialize populations");
    feature_extractor_flight flights(db_conn_str_);
    features_weather         weather(db_conn_str_);
    vector<shared_ptr<solution_config> > solutions = solution_config::get_initial_generation(site_name, db_conn_str_);
    const bgreg::date_period dates = weather.get_feature_date_period(false);

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


    for(bgreg::day_iterator dit(dates.begin()); *dit <= dates.end(); ++dit)
    {
        if(used_for_training(*dit))
        {

        }

        BOOST_FOREACH(shared_ptr<solution_config> solution, solutions)
        {

        }

    }



    for(bgreg::day_iterator dit(dates.begin()); *dit <= dates.end(); ++dit)
    {
        if(used_for_validation(*dit))
        {

        }

    }

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const bool solution_manager::used_for_training(const bgreg::date &day) const
{
    return day.day_of_week() == 0; // sunday
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
const bool solution_manager::used_for_validation(const bgreg::date &day) const
{
    return day.day_of_week() == 6; // saturday
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
