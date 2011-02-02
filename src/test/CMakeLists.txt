PROJECT(flightpred_test)
SET(CMAKE_MODULE_PATH ${flightpred_test_SOURCE_DIR})

ADD_EXECUTABLE(flightpred_test main.cpp serialization_test.cpp)
TARGET_LINK_LIBRARIES(flightpred_test flightpred_common boost_filesystem-mt boost_date_time-mt boost_system-mt boost_thread-mt boost_regex-mt boost_serialization-mt boost_unit_test_framework-mt boost_iostreams-mt pqxx grib_api evocosm coyotl)

SET_TARGET_PROPERTIES(
    flightpred_test
  PROPERTIES
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    DEBUG_POSTFIX "d"
  )

