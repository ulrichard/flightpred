// flightpred
#include "common/solution_config.h"
#include "common/features_weather.h"
#include "common/lm_svm_dlib.h"
#include "common/GenGeomLibSerialize.h"
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

#include <boost/lambda/bind.hpp>
#include <boost/multi_index/sequenced_index.hpp>

// std lib
#include <iostream>
#include <sstream>

using namespace flightpred;
using namespace boost::unit_test;
namespace bfs = boost::filesystem;
using boost::geometry::point_ll_deg;
using boost::geometry::latitude;
using boost::geometry::longitude;
using std::string;
using std::vector;

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
BOOST_AUTO_TEST_CASE(SolutionSerialize)
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

