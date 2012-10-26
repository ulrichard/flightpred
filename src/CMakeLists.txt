
FIND_PACKAGE(Boost 1.42 COMPONENTS system filesystem iostreams unit_test_framework date_time thread regex serialization program_options signals REQUIRED)

ADD_DEFINITIONS(-DWT_THREADED -D_REENTRANT -DBOOST_SPIRIT_THREADSAFE)

# Make sure the compiler can find include files.
INCLUDE_DIRECTORIES(${flightpred_MAIN_DIR}/3rd-party/ggl)
INCLUDE_DIRECTORIES(${flightpred_MAIN_DIR}/3rd-party/json_spirit/json_spirit)
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SUBDIRS(common train web test)



