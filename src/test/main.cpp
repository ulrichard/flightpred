// flightpred
#include "../common/solution_config.h"
#include "../common/features_weather.h"
#include "../common/lm_svm_dlib.h"
#include "../common/grab_grib.h"
#include "../common/solution_manager.h"
#include "../common/train_svm.h"
#include "../common/GenGeomLibSerialize.h"
#include "../common/reporter.h"
// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/io/wkt/write_wkt.hpp>
// boost
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <boost/lambda/bind.hpp>
#include <boost/multi_index/sequenced_index.hpp>

// std lib
#include <iostream>
#include <sstream>

using namespace flightpred;
using namespace flightpred::reporting;
using namespace boost::unit_test;
namespace bfs = boost::filesystem;
namespace bgreg = boost::gregorian;
using boost::geometry::point_ll_deg;
using boost::geometry::latitude;
using boost::geometry::longitude;
using std::string;
using std::vector;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
struct MyConfig
{
    MyConfig()
        : dbname_("flightpred_test")
    {
        const bfs::path scriptdir(bfs::path(__FILE__).parent_path().parent_path().parent_path() / "scripts");
        const bfs::path backupdir(bfs::path(__FILE__).parent_path().parent_path().parent_path() / "backup");

        ReportDispatcher::inst().add(new ListenerCout(), DEBUGING);

#ifndef SKIP_FLPR_DB_SETUP
        // set up the database
        std::stringstream sstr;
        sstr << "psql -e -a -c \"DROP DATABASE IF EXISTS " << dbname_ << "\"";
        int ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        sstr.str("");
        sstr << "createdb -e " << dbname_ << "";
        ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        sstr.str("");
        sstr << "createlang plpgsql " << dbname_ << "";
        ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        sstr.str("");
        sstr << "psql -e -a -d " << dbname_ << " -f /usr/share/postgresql/8.4/contrib/postgis-1.5/postgis.sql";
        ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        sstr.str("");
        sstr << "psql -e -a -d " << dbname_ << " -f /usr/share/postgresql/8.4/contrib/postgis-1.5/spatial_ref_sys.sql";
        ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        sstr.str("");
        sstr << "psql -e -a -d " << dbname_ << " -f " << (scriptdir / "create_flightpred_db.sql").external_file_string();
        ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        // add some flights
        sstr.str("");
        sstr << "psql -e -a -d " << dbname_ << " -c \""
             << "INSERT INTO pilots  (pilot_name, country) VALUES ('Richard Ulrich', 'CH');"
             << "INSERT INTO sites   (pred_site_id, site_name, country) VALUES (4, 'Rothenflue', 'CH');"
             << "INSERT INTO flights (pilot_id, contest_id, site_id, flight_date, distance, score, duration) VALUES (1, 1, 1, '2009-09-03', 52, 52, 65);"
             << "INSERT INTO flights (pilot_id, contest_id, site_id, flight_date, distance, score, duration) VALUES (1, 1, 1, '2009-09-03', 25, 25, 30);"
             << "INSERT INTO flights (pilot_id, contest_id, site_id, flight_date, distance, score, duration) VALUES (1, 1, 1, '2009-09-04', 48, 48, 130);"
             << "INSERT INTO flights (pilot_id, contest_id, site_id, flight_date, distance, score, duration) VALUES (1, 1, 1, '2009-09-05', 37, 37, 99);"
             << "\"";
        ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        // download weather data
        const char* user = getenv("USER");
        const string db_conn_str = "host=localhost port=5432 dbname=" + dbname_ + " user=flightpred password=flightpred";
        flightpred_db::init(db_conn_str);

        grib_grabber_gfs_past gr(50, true);
        gr.grab_grib(bgreg::from_string("2009-09-01"), bgreg::from_string("2009-09-07"));

#else

        const char* user = getenv("USER");
        const string db_conn_str = "host=localhost port=5432 dbname=" + dbname_ + " user=flightpred password=flightpred";
        flightpred_db::init(db_conn_str);

#endif // SKIP_FLPR_DB_SETUP

    }

#ifndef SKIP_FLPR_DB_SETUP
    ~MyConfig()
    {
        std::stringstream sstr;
        sstr << "psql -e -a -c \"DROP DATABASE IF EXISTS " << dbname_ << "\"";
        int ret = system(sstr.str().c_str());
        std::cout << "dropping the db returned: " << ret;
    }
#endif // SKIP_FLPR_DB_SETUP

private:
    const std::string dbname_;
};


BOOST_GLOBAL_FIXTURE(MyConfig);
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class Fruit
{
public:
    Fruit(const std::string& nam) : name_(nam) {}
    const std::string& name() { return name_; }
private:
    std::string name_;
};

BOOST_AUTO_TEST_CASE(multi_index_tst)
{

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(SolutionParser_DLIB_KRLS)
{
    static const string soldescr("DLIB_KRLS(RBF(0.01 )1e-05)   FEATURE(GFS -12:00:00 POINT(8 46) 0 PRES) FEATURE(GFS -12:00:00 POINT(8 47) 0 PRES)");
    solution_config solconf("TestBerg", soldescr, 0);

    BOOST_CHECK_EQUAL("DLIB_KRLS(RBF(0.01 )1e-05) with 2 features", solconf.get_short_description());
    BOOST_CHECK_EQUAL("DLIB_KRLS(RBF(0.01 )1e-05)", solconf.get_algorithm_name(true));
    BOOST_CHECK_EQUAL("DLIB_KRLS(RBF)", solconf.get_algorithm_name(false));
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(SerializeSolutionConfiguration)
{
    const string soldescr("DLIB_KRLS(RBF(0.01 )1e-05)   FEATURE(GFS -12:00:00 POINT(8 46) 0 PRES) FEATURE(GFS -12:00:00 POINT(8 47) 0 PRES)");
    const solution_config solconf("Rothenflue", soldescr, 0);
    const string country("Switzerland");
    const point_ll_deg posSchwyz(latitude<>(47.032228), longitude<>(8.653793));

    const bfs::path backup_dir("/tmp/flightpred/tests");
    bfs::create_directories(backup_dir);
    bfs::path outfile(backup_dir / (solconf.get_site_name() + ".flightpred"));

    if(true)
    {
        bfs::ofstream ofs(outfile, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        if(!ofs.good())
            throw std::runtime_error("could not write to " + outfile.string());
        boost::archive::binary_oarchive oa(ofs);

        // information about the flying site
        oa << solconf.get_site_name();
        oa << country;
        oa << posSchwyz;

        // register all derived algoritms that are in use
        oa.register_type<lm_dlib_rvm<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > >();
        oa.register_type<lm_dlib_rvm<dlib::sigmoid_kernel<dlib::matrix<double, 0, 1> > > >();
        oa.register_type<lm_dlib_rvm<dlib::polynomial_kernel<dlib::matrix<double, 0, 1> > > >();
        oa.register_type<lm_dlib_krls<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > >();

        // serialize the solution itself
        const solution_config *sc = &solconf;
        oa << sc;
    }

    if(true)
    {
        bfs::ifstream ifs(outfile, std::ios_base::in | std::ios_base::binary);
        if(!ifs.good())
            throw std::runtime_error("file not found : " + outfile.string());

        boost::archive::binary_iarchive ia(ifs);
        // read class state from archive
        string site_name;
        ia >> site_name;
        BOOST_CHECK_EQUAL(site_name, solconf.get_site_name());

        string cntr;
        ia >> cntr;
        BOOST_CHECK_EQUAL(country, cntr);

        point_ll_deg location;
        ia >> location;
        BOOST_CHECK_EQUAL(posSchwyz, location);

        // register all derived algoritms
        ia.register_type<lm_dlib_rvm<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > >();
        ia.register_type<lm_dlib_rvm<dlib::sigmoid_kernel<dlib::matrix<double, 0, 1> > > >();
        ia.register_type<lm_dlib_rvm<dlib::polynomial_kernel<dlib::matrix<double, 0, 1> > > >();
        ia.register_type<lm_dlib_krls<dlib::radial_basis_kernel<dlib::matrix<double, 0, 1> > > >();

        solution_config *sol = 0;
        ia >> sol;

        BOOST_REQUIRE(sol);
        BOOST_CHECK_EQUAL(solconf.get_generation(), sol->get_generation());
        BOOST_CHECK_EQUAL(solconf.get_site_name(), sol->get_site_name());
        BOOST_CHECK_EQUAL(solconf.get_description(), sol->get_description());


//        solconf.get_decision_function("num_flight");

    }


}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(SerializeTrainedSolution)
{
    const std::string site_name("Rothenflue");
    std::stringstream sstr;

    // clear the trained_solutions
    pqxx::transaction<> trans1(flightpred_db::get_conn(), "SerializeTrainedSolution test");
    trans1.exec("delete from trained_solutions where pred_site_id=4");

    // insert a solution
    sstr << "insert into trained_solutions (pred_site_id, configuration, validation_error, train_time, generation, num_features) values ("
         << "4, 'DLIB_RVM(RBF(0.02 ))  "
         << "FEATURE(GFS -12:00:00 POINT(6 47) 0 PRES) FEATURE(GFS -12:00:00 POINT(7 46) 0 PRES) FEATURE(GFS -12:00:00 POINT(7 47) 0 PRES) "
         << "FEATURE(GFS -12:00:00 POINT(8 46) 0 PRES) FEATURE(GFS -12:00:00 POINT(8 47) 0 PRES) FEATURE(GFS -12:00:00 POINT(8 48) 0 PRES) "
         << "FEATURE(GFS -12:00:00 POINT(9 45) 0 PRES) FEATURE(GFS -12:00:00 POINT(9 46) 0 PRES) FEATURE(GFS -12:00:00 POINT(9 47) 0 PRES) "
         << "FEATURE(GFS -12:00:00 POINT(9 48) 0 PRES) FEATURE(GFS -12:00:00 POINT(9 49) 0 PRES) FEATURE(GFS -12:00:00 POINT(10 46) 0 PRES) "
         << "FEATURE(GFS -12:00:00 POINT(10 47) 0 PRES) FEATURE(GFS -12:00:00 POINT(10 48) 0 PRES) FEATURE(GFS -12:00:00 POINT(11 46) 0 PRES) "
         << "FEATURE(GFS -12:00:00 POINT(11 47) 0 PRES) FEATURE(GFS -12:00:00 POINT(11 48) 0 PRES) FEATURE(GFS -06:00:00 POINT(6 47) 0 PRES) "
         << "FEATURE(GFS -06:00:00 POINT(7 46) 0 PRES) FEATURE(GFS -06:00:00 POINT(7 47) 0 PRES) FEATURE(GFS -06:00:00 POINT(7 48) 0 PRES) "
         << "FEATURE(GFS -06:00:00 POINT(8 46) 0 PRES) FEATURE(GFS -06:00:00 POINT(8 47) 0 PRES) FEATURE(GFS -06:00:00 POINT(8 48) 0 PRES) "
         << "FEATURE(GFS -06:00:00 POINT(9 45) 0 PRES) FEATURE(GFS -06:00:00 POINT(9 46) 0 PRES) FEATURE(GFS -06:00:00 POINT(9 47) 0 PRES) "
         << "FEATURE(GFS -06:00:00 POINT(9 48) 0 PRES) FEATURE(GFS -06:00:00 POINT(9 49) 0 PRES) FEATURE(GFS -06:00:00 POINT(10 46) 0 PRES) "
         << "FEATURE(GFS -06:00:00 POINT(10 47) 0 PRES) FEATURE(GFS -06:00:00 POINT(10 48) 0 PRES) FEATURE(GFS -06:00:00 POINT(11 46) 0 PRES) "
         << "FEATURE(GFS -06:00:00 POINT(11 47) 0 PRES) FEATURE(GFS -06:00:00 POINT(11 48) 0 PRES) FEATURE(GFS 00:00:00 POINT(6 47) 0 PRES) "
         << "FEATURE(GFS 00:00:00 POINT(7 46) 0 PRES) FEATURE(GFS 00:00:00 POINT(7 47) 0 PRES) FEATURE(GFS 00:00:00 POINT(7 48) 0 PRES) "
         << "FEATURE(GFS 00:00:00 POINT(8 46) 0 PRES) FEATURE(GFS 00:00:00 POINT(8 47) 0 PRES) FEATURE(GFS 00:00:00 POINT(8 48) 0 PRES) "
         << "FEATURE(GFS 00:00:00 POINT(9 45) 0 PRES) FEATURE(GFS 00:00:00 POINT(9 46) 0 PRES) FEATURE(GFS 00:00:00 POINT(9 47) 0 PRES) "
         << "FEATURE(GFS 00:00:00 POINT(9 48) 0 PRES) FEATURE(GFS 00:00:00 POINT(9 49) 0 PRES) FEATURE(GFS 00:00:00 POINT(10 47) 0 PRES) "
         << "FEATURE(GFS 00:00:00 POINT(10 48) 0 PRES) FEATURE(GFS 00:00:00 POINT(11 46) 0 PRES) FEATURE(GFS 00:00:00 POINT(11 47) 0 PRES) "
         << "FEATURE(GFS 00:00:00 POINT(11 48) 0 PRES) FEATURE(GFS 06:00:00 POINT(6 47) 0 PRES) FEATURE(GFS 06:00:00 POINT(7 46) 0 PRES) "
         << "FEATURE(GFS 06:00:00 POINT(7 47) 0 PRES) FEATURE(GFS 06:00:00 POINT(7 48) 0 PRES) FEATURE(GFS 06:00:00 POINT(8 46) 0 PRES) "
         << "FEATURE(GFS 06:00:00 POINT(8 47) 0 PRES) FEATURE(GFS 06:00:00 POINT(8 48) 0 PRES) FEATURE(GFS 06:00:00 POINT(9 45) 0 PRES) "
         << "FEATURE(GFS 06:00:00 POINT(9 46) 0 PRES) FEATURE(GFS 06:00:00 POINT(9 47) 0 PRES) FEATURE(GFS 06:00:00 POINT(9 48) 0 PRES) "
         << "FEATURE(GFS 06:00:00 POINT(9 49) 0 PRES) FEATURE(GFS 06:00:00 POINT(10 46) 0 PRES) FEATURE(GFS 06:00:00 POINT(10 47) 0 PRES) "
         << "FEATURE(GFS 06:00:00 POINT(10 48) 0 PRES) FEATURE(GFS 06:00:00 POINT(11 46) 0 PRES) FEATURE(GFS 06:00:00 POINT(11 47) 0 PRES) "
         << "FEATURE(GFS 06:00:00 POINT(11 48) 0 PRES) FEATURE(GFS 12:00:00 POINT(6 47) 0 PRES) FEATURE(GFS 12:00:00 POINT(7 46) 0 PRES) "
         << "FEATURE(GFS 12:00:00 POINT(7 47) 0 PRES) FEATURE(GFS 12:00:00 POINT(7 48) 0 PRES) FEATURE(GFS 12:00:00 POINT(8 46) 0 PRES) "
         << "FEATURE(GFS 12:00:00 POINT(8 47) 0 PRES) FEATURE(GFS 12:00:00 POINT(8 48) 0 PRES) FEATURE(GFS 12:00:00 POINT(9 45) 0 PRES) "
         << "FEATURE(GFS 12:00:00 POINT(9 46) 0 PRES) FEATURE(GFS 12:00:00 POINT(9 47) 0 PRES) FEATURE(GFS 12:00:00 POINT(9 48) 0 PRES) "
         << "FEATURE(GFS 12:00:00 POINT(9 49) 0 PRES) FEATURE(GFS 12:00:00 POINT(10 46) 0 PRES) FEATURE(GFS 12:00:00 POINT(10 47) 0 PRES) "
         << "FEATURE(GFS 12:00:00 POINT(10 48) 0 PRES) FEATURE(GFS 12:00:00 POINT(11 46) 0 PRES) FEATURE(GFS 12:00:00 POINT(11 47) 0 PRES) "
         << "FEATURE(GFS 12:00:00 POINT(11 48) 0 PRES) FEATURE(GFS 18:00:00 POINT(6 47) 0 PRES) FEATURE(GFS 18:00:00 POINT(7 46) 0 PRES) "
         << "FEATURE(GFS 18:00:00 POINT(7 47) 0 PRES) FEATURE(GFS 18:00:00 POINT(7 48) 0 PRES) FEATURE(GFS 18:00:00 POINT(8 46) 0 PRES) "
         << "FEATURE(GFS 18:00:00 POINT(8 47) 0 PRES) FEATURE(GFS 18:00:00 POINT(9 45) 0 PRES) FEATURE(GFS 18:00:00 POINT(9 46) 0 PRES) "
         << "FEATURE(GFS 18:00:00 POINT(9 47) 0 PRES) FEATURE(GFS 18:00:00 POINT(9 48) 0 PRES) FEATURE(GFS 18:00:00 POINT(9 49) 0 PRES) "
         << "FEATURE(GFS 18:00:00 POINT(10 46) 0 PRES) FEATURE(GFS 18:00:00 POINT(10 47) 0 PRES) FEATURE(GFS 18:00:00 POINT(10 48) 0 PRES)', "
         << 122.157 << ", " << 0.13 << ", " << 10 << ", " << 52 << ")";
    trans1.exec(sstr.str());
    trans1.commit();

    // train
    vector<string> vfigures;
    vfigures.push_back("num_flight");
    vfigures.push_back("max_dist");
    vfigures.push_back("avg_dist");
    vfigures.push_back("max_dur");
    vfigures.push_back("avg_dur");
    train_svm trainer;
    trainer.train(site_name, bgreg::from_string("2009-09-02"), bgreg::from_string("2009-09-05"), 1, vfigures);

    // check
    solution_manager mgr(site_name);
    {
        pqxx::transaction<> trans2(flightpred_db::get_conn(), "SerializeTrainedSolution test");
        pqxx::result res = trans2.exec("select num_flight, train_time_prod, num_samples_prod from trained_solutions where pred_site_id=4");
        BOOST_REQUIRE_GE(res.size(), 1);
        int oid_num_flight;
        res[0][0].to(oid_num_flight);
        BOOST_CHECK_GT(oid_num_flight, 0);
        trans2.commit();

        std::auto_ptr<solution_config> sol = mgr.load_best_solution(true);
        BOOST_CHECK_EQUAL(10, sol->get_generation());
        BOOST_CHECK_EQUAL("Rothenflue", sol->get_site_name());
        BOOST_CHECK_GE(sol->get_weather_feature_desc().size(), 100);
        BOOST_CHECK_GE(sol->get_decision_function("num_flight")->main_metric(), 3);
        BOOST_CHECK_GE(sol->get_decision_function("max_dist")->main_metric(), 3);
        BOOST_CHECK_GE(sol->get_decision_function("avg_dist")->main_metric(), 3);
        BOOST_CHECK_GE(sol->get_decision_function("max_dur")->main_metric(), 3);
        BOOST_CHECK_GE(sol->get_decision_function("avg_dur")->main_metric(), 3);
    }

    // export
    const bfs::path backup_dir("/tmp/flightpred_test");
    const solution_manager::BackupFormat bfmt = solution_manager::BAK_TEXT;

    mgr.export_solution(backup_dir, bfmt);
    BOOST_REQUIRE(bfs::exists(backup_dir / "Rothenflue.flightpred"));

    //  clear from the db
    pqxx::transaction<> trans3(flightpred_db::get_conn(), "SerializeTrainedSolution test");
    trans3.exec("delete from trained_solutions where pred_site_id=4");
    trans3.commit();

    // import
    solution_manager::import_solution(backup_dir, site_name);

    // check
    pqxx::transaction<> trans4(flightpred_db::get_conn(), "SerializeTrainedSolution test");
    pqxx::result res = trans4.exec("select num_flight, train_time_prod, num_samples_prod from trained_solutions where pred_site_id=4");
    BOOST_REQUIRE_GE(res.size(), 1);
    int oid_num_flight;
    res[0][0].to(oid_num_flight);
    BOOST_CHECK_GT(oid_num_flight, 0);
    trans4.commit();

    std::auto_ptr<solution_config> sol = mgr.load_best_solution(true);
    BOOST_CHECK_EQUAL(10, sol->get_generation());
    BOOST_CHECK_EQUAL(site_name, sol->get_site_name());
    BOOST_CHECK_GE(sol->get_weather_feature_desc().size(), 100);
    BOOST_CHECK_GE(sol->get_decision_function("num_flight")->main_metric(), 3);
    BOOST_CHECK_GE(sol->get_decision_function("max_dist")->main_metric(), 3);
    BOOST_CHECK_GE(sol->get_decision_function("avg_dist")->main_metric(), 3);
    BOOST_CHECK_GE(sol->get_decision_function("max_dur")->main_metric(), 3);
    BOOST_CHECK_GE(sol->get_decision_function("avg_dur")->main_metric(), 3);


}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(ImportUnknownSolution)
{
    const std::string site_name("Emberger Alm");
    const bfs::path backupdir(bfs::path(__FILE__).parent_path().parent_path().parent_path() / "backup");
    std::stringstream sstr;

    solution_manager::import_solution(backupdir, site_name);

    // check
    pqxx::transaction<> trans4(flightpred_db::get_conn(), "ImportUnknownSolution test");
    pqxx::result res = trans4.exec("select num_flight, train_time_prod, num_samples_prod from trained_solutions inner join pred_sites "
                                    "on trained_solutions.pred_site_id = pred_sites.pred_site_id where site_name='" + site_name + "' order by train_time_prod desc");
    BOOST_REQUIRE_GE(res.size(), 1);
    int oid_num_flight;
    res[0][0].to(oid_num_flight);
    BOOST_CHECK_NE(oid_num_flight, 0);
    trans4.commit();

    solution_manager mgr(site_name);
    std::auto_ptr<solution_config> sol = mgr.load_best_solution(true);
    BOOST_CHECK_EQUAL(0, sol->get_generation());
    BOOST_CHECK_EQUAL(site_name, sol->get_site_name());
    BOOST_CHECK_EQUAL(sol->get_weather_feature_desc().size(), 528);
    BOOST_CHECK_EQUAL(sol->get_decision_function("num_flight")->main_metric(), 34);
    BOOST_CHECK_EQUAL(sol->get_decision_function("max_dist")->main_metric(), 41);
    BOOST_CHECK_EQUAL(sol->get_decision_function("avg_dist")->main_metric(), 42);
    BOOST_CHECK_EQUAL(sol->get_decision_function("max_dur")->main_metric(), 49);
    BOOST_CHECK_EQUAL(sol->get_decision_function("avg_dur")->main_metric(), 47);

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void locations_around_site_testfunc(const point_ll_deg& pos, const size_t num, const double grid, const vector<point_ll_deg> expected)
{
    vector<point_ll_deg> pnts = features_weather::get_locations_around_site(pos, num, grid);

    BOOST_CHECK_EQUAL(expected.size(), pnts.size());
    BOOST_FOREACH(const point_ll_deg& pos, expected)
    {
        vector<point_ll_deg>::iterator fit = std::find(pnts.begin(), pnts.end(), pos);
        if(fit != pnts.end())
            pnts.erase(fit);
        else
        {
            std::stringstream sstr;
            sstr << boost::geometry::make_wkt(pos);
            BOOST_CHECK_EQUAL(sstr.str(), "ExpectedButNotFound");
        }
    }
    BOOST_CHECK_EQUAL(0, pnts.size());
    BOOST_FOREACH(const point_ll_deg& pos, pnts)
    {
        std::stringstream sstr;
        sstr << boost::geometry::make_wkt(pos);
        BOOST_CHECK_EQUAL(sstr.str(), "FoundButNotExpected");
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(locations_around_site_Schwyz_4_10)
{
    const point_ll_deg posSchwyz(latitude<>(47.032228), longitude<>(8.653793));
    vector<point_ll_deg> expected;
    expected.push_back(point_ll_deg(latitude<>(47.0), longitude<>(8.0)));
//    expected.push_back(point_ll_deg(latitude<>(48.0), longitude<>(8.0)));
    expected.push_back(point_ll_deg(latitude<>(48.0), longitude<>(9.0)));
    expected.push_back(point_ll_deg(latitude<>(47.0), longitude<>(9.0)));
    expected.push_back(point_ll_deg(latitude<>(46.0), longitude<>(9.0)));


    locations_around_site_testfunc(posSchwyz, 4, 1.0, expected);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(locations_around_site_Schwyz_16_25)
{
    const point_ll_deg posSchwyz(latitude<>(47.032228), longitude<>(8.653793));
    vector<point_ll_deg> expected;
    expected.push_back(point_ll_deg(latitude<>(47.5), longitude<>( 7.5)));
    expected.push_back(point_ll_deg(latitude<>(47.5), longitude<>(10.0)));
    expected.push_back(point_ll_deg(latitude<>(45.0), longitude<>( 7.5)));
    expected.push_back(point_ll_deg(latitude<>(45.0), longitude<>(10.0)));
    expected.push_back(point_ll_deg(latitude<>(47.5), longitude<>( 5.0)));
    expected.push_back(point_ll_deg(latitude<>(47.5), longitude<>(12.5)));
    expected.push_back(point_ll_deg(latitude<>(50.0), longitude<>( 7.5)));
    expected.push_back(point_ll_deg(latitude<>(50.0), longitude<>(10.0)));
    expected.push_back(point_ll_deg(latitude<>(45.0), longitude<>( 5.0)));
    expected.push_back(point_ll_deg(latitude<>(45.0), longitude<>(12.5)));
    expected.push_back(point_ll_deg(latitude<>(50.0), longitude<>( 5.0)));
    expected.push_back(point_ll_deg(latitude<>(50.0), longitude<>(12.5)));
    expected.push_back(point_ll_deg(latitude<>(47.5), longitude<>( 2.5)));
    expected.push_back(point_ll_deg(latitude<>(47.5), longitude<>(15.0)));
    expected.push_back(point_ll_deg(latitude<>(42.5), longitude<>( 7.5)));
    expected.push_back(point_ll_deg(latitude<>(42.5), longitude<>(10.0)));

    locations_around_site_testfunc(posSchwyz, 16, 2.5, expected);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(locations_around_site_DeAar_4_10)
{
    const point_ll_deg posDeAar(latitude<>(-30.6979), longitude<>(24.0271));
    vector<point_ll_deg> expected;
    expected.push_back(point_ll_deg(latitude<>(-31.0), longitude<>(24.0)));
    expected.push_back(point_ll_deg(latitude<>(-30.0), longitude<>(24.0)));
    expected.push_back(point_ll_deg(latitude<>(-31.0), longitude<>(25.0)));
//  expected.push_back(point_ll_deg(latitude<>(-30.0), longitude<>(25.0)));
    expected.push_back(point_ll_deg(latitude<>(-31.0), longitude<>(23.0)));

    locations_around_site_testfunc(posDeAar, 4, 1.0, expected);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(locations_around_site_Piedrahita_4_10)
{
    const point_ll_deg posPiedrahita(latitude<>(40.4224), longitude<>(-4.99504));
    vector<point_ll_deg> expected;
    expected.push_back(point_ll_deg(latitude<>(40.0), longitude<>(-5.0)));
    expected.push_back(point_ll_deg(latitude<>(41.0), longitude<>(-5.0)));
    expected.push_back(point_ll_deg(latitude<>(40.0), longitude<>(-4.0)));
//  expected.push_back(point_ll_deg(latitude<>(41.0), longitude<>(-4.0)));
    expected.push_back(point_ll_deg(latitude<>(40.0), longitude<>(-6.0)));

    locations_around_site_testfunc(posPiedrahita, 4, 1.0, expected);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(locations_around_site_Valadares_4_10)
{
    const point_ll_deg posValadares(latitude<>(-18.8865), longitude<>(-41.9151));
    vector<point_ll_deg> expected;
    expected.push_back(point_ll_deg(latitude<>(-19.0), longitude<>(-41.0)));
    expected.push_back(point_ll_deg(latitude<>(-19.0), longitude<>(-42.0)));
    expected.push_back(point_ll_deg(latitude<>(-18.0), longitude<>(-42.0)));
//  expected.push_back(point_ll_deg(latitude<>(-18.0), longitude<>(-41.0)));
    expected.push_back(point_ll_deg(latitude<>(-19.0), longitude<>(-43.0)));

    locations_around_site_testfunc(posValadares, 4, 1.0, expected);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

