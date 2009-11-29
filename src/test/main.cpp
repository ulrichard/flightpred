// flightpred
#include "common/solution_configuration.h"
// boost
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
// std lib
#include <iostream>

using namespace boost::unit_test;

/*
int main()
{
    cout << "Hello world!" << endl;
    return 0;
}
*/
class solution_config_tester : public flightpred::solution_config
{
    solution_config_tester() {}
    virtual ~solution_config_tester() {}
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(SolutionParser)
{
    bfs::path igcfile = flb::SystemInformation::homeDir() / "gipsy/Richard Ulrich/2008/2008-09-13-XPG-RIC-01.igc";
    BOOST_REQUIRE(bfs::exists(igcfile));
    shared_ptr<flb::FlightDatabase> fldb(new flb::FlightDatabase("tester"));
    flb::inout_igc ioigc(fldb);
    ioigc.read(igcfile);

    BOOST_CHECK_EQUAL(ioigc.pilotName(),        "Richard Ulrich");
    BOOST_CHECK_EQUAL(ioigc.gliderName(),       "MAC PARA Magus 5");
    BOOST_CHECK_EQUAL(ioigc.callsign(),         "");
    BOOST_CHECK_EQUAL(ioigc.takeoffName(),      "Piedrahita Pena Negra (ES)");
    BOOST_CHECK_EQUAL(ioigc.comment(),          "Deutsche Meisterschaft. Einziger gueltiger Task");
    BOOST_CHECK_EQUAL(ioigc.date(),             boost::gregorian::date(2008, 9, 13));
    BOOST_CHECK_EQUAL(ioigc.Trackpoints.size(), 2981);


}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
