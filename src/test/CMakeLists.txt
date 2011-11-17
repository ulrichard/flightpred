PROJECT(flightpred_test)
SET(CMAKE_MODULE_PATH ${flightpred_test_SOURCE_DIR})

ADD_EXECUTABLE(flightpred_test main.cpp serialization_test.cpp)
TARGET_LINK_LIBRARIES(flightpred_test 
	flightpred_common 
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_DATE_TIME_LIBRARY}
	${Boost_THREAD_LIBRARY}
	${Boost_REGEX_LIBRARY}
	${Boost_SERIALIZATION_LIBRARY}
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	${Boost_IOSTREAMS_LIBRARY} 
	pqxx 
	grib_api 
	evocosm 
	coyotl
)

SET_TARGET_PROPERTIES(
    flightpred_test
  PROPERTIES
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    DEBUG_POSTFIX "d"
  )

