// flightpred
#include "common/solution_config.h"
#include "common/features_weather.h"
// ggl (boost sandbox)
#include <geometry/io/wkt/aswkt.hpp>
// boost
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
// std lib
#include <iostream>
#include <sstream>

using namespace flightpred;
using namespace boost::unit_test;
namespace bfs = boost::filesystem;
using geometry::point_ll_deg;
using std::string;
using std::vector;

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
    static const string soldescr("DLIB_KRLS(RBF(0.01 )1e-05)   FEATURE(GFS -12:00:00 POINT(8 46) 0 PRES) FEATURE(GFS -12:00:00 POINT(8 47) 0 PRES)");
    solution_config solconf("Rothenflue", soldescr, 0);



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
            sstr << geometry::make_wkt(pos);
            BOOST_CHECK_EQUAL(sstr.str(), "ExpectedButNotFound");
        }
    }
    BOOST_CHECK_EQUAL(0, pnts.size());
    BOOST_FOREACH(const point_ll_deg& pos, pnts)
    {
        std::stringstream sstr;
        sstr << geometry::make_wkt(pos);
        BOOST_CHECK_EQUAL(sstr.str(), "FountButNotExpected");
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(locations_around_site_Schwyz_4_10)
{
    point_ll_deg posSchwyz(geometry::latitude<>(47.032228), geometry::longitude<>(8.653793));
    vector<point_ll_deg> expected;
    expected.push_back(point_ll_deg(geometry::latitude<>(47.0), geometry::longitude<>(8.0)));
//    expected.push_back(point_ll_deg(geometry::latitude<>(48.0), geometry::longitude<>(8.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(48.0), geometry::longitude<>(9.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(47.0), geometry::longitude<>(9.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(46.0), geometry::longitude<>(9.0)));


    locations_around_site_testfunc(posSchwyz, 4, 1.0, expected);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(locations_around_site_Schwyz_16_25)
{
    point_ll_deg posSchwyz(geometry::latitude<>(47.032228), geometry::longitude<>(8.653793));
    vector<point_ll_deg> expected;
    expected.push_back(point_ll_deg(geometry::latitude<>(47.5), geometry::longitude<>( 7.5)));
    expected.push_back(point_ll_deg(geometry::latitude<>(47.5), geometry::longitude<>(10.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(45.0), geometry::longitude<>( 7.5)));
    expected.push_back(point_ll_deg(geometry::latitude<>(45.0), geometry::longitude<>(10.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(47.5), geometry::longitude<>( 5.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(47.5), geometry::longitude<>(12.5)));
    expected.push_back(point_ll_deg(geometry::latitude<>(50.0), geometry::longitude<>( 7.5)));
    expected.push_back(point_ll_deg(geometry::latitude<>(50.0), geometry::longitude<>(10.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(45.0), geometry::longitude<>( 5.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(45.0), geometry::longitude<>(12.5)));
    expected.push_back(point_ll_deg(geometry::latitude<>(50.0), geometry::longitude<>( 5.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(50.0), geometry::longitude<>(12.5)));
    expected.push_back(point_ll_deg(geometry::latitude<>(47.5), geometry::longitude<>( 2.5)));
    expected.push_back(point_ll_deg(geometry::latitude<>(47.5), geometry::longitude<>(15.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(42.5), geometry::longitude<>( 7.5)));
    expected.push_back(point_ll_deg(geometry::latitude<>(42.5), geometry::longitude<>(10.0)));

    locations_around_site_testfunc(posSchwyz, 16, 2.5, expected);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(locations_around_site_DeAar_4_10)
{
    point_ll_deg posDeAar(geometry::latitude<>(-30.6979), geometry::longitude<>(24.0271));
    vector<point_ll_deg> expected;
    expected.push_back(point_ll_deg(geometry::latitude<>(-31.0), geometry::longitude<>(24.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(-30.0), geometry::longitude<>(24.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(-31.0), geometry::longitude<>(25.0)));
//  expected.push_back(point_ll_deg(geometry::latitude<>(-30.0), geometry::longitude<>(25.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(-31.0), geometry::longitude<>(23.0)));

    locations_around_site_testfunc(posDeAar, 4, 1.0, expected);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(locations_around_site_Piedrahita_4_10)
{
    point_ll_deg posPiedrahita(geometry::latitude<>(40.4224), geometry::longitude<>(-4.99504));
    vector<point_ll_deg> expected;
    expected.push_back(point_ll_deg(geometry::latitude<>(40.0), geometry::longitude<>(-5.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(41.0), geometry::longitude<>(-5.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(40.0), geometry::longitude<>(-4.0)));
//  expected.push_back(point_ll_deg(geometry::latitude<>(41.0), geometry::longitude<>(-4.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(40.0), geometry::longitude<>(-6.0)));

    locations_around_site_testfunc(posPiedrahita, 4, 1.0, expected);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(locations_around_site_Valadares_4_10)
{
    point_ll_deg posValadares(geometry::latitude<>(-18.8865), geometry::longitude<>(-41.9151));
    vector<point_ll_deg> expected;
    expected.push_back(point_ll_deg(geometry::latitude<>(-19.0), geometry::longitude<>(-41.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(-19.0), geometry::longitude<>(-42.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(-18.0), geometry::longitude<>(-42.0)));
//  expected.push_back(point_ll_deg(geometry::latitude<>(-18.0), geometry::longitude<>(-41.0)));
    expected.push_back(point_ll_deg(geometry::latitude<>(-19.0), geometry::longitude<>(-43.0)));

    locations_around_site_testfunc(posValadares, 4, 1.0, expected);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

