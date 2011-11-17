CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(flightpred)
SET(CMAKE_MODULE_PATH ${flightpred_SOURCE_DIR})

FIND_PACKAGE(Boost 1.42 COMPONENTS system filesystem iostreams unit_test_framework date_time thread regex serialization program_options signals REQUIRED)

ADD_DEFINITIONS(-DWT_THREADED -D_REENTRANT -DBOOST_SPIRIT_THREADSAFE)

# Make sure the compiler can find include files.
include_directories (${PROJECT_SOURCE_DIR}) 
include_directories (${PROJECT_SOURCE_DIR}/common) 
include_directories (${PROJECT_SOURCE_DIR}/train)
include_directories (${PROJECT_SOURCE_DIR}/web)
include_directories (${PROJECT_SOURCE_DIR}/../3rd-party/ggl)
include_directories (${PROJECT_SOURCE_DIR}/../3rd-party/json_spirit/json_spirit)
include_directories (${PROJECT_SOURCE_DIR}/../3rd-party/dclib)
include_directories (${BOOST_INCLUDE_DIRS})

LINK_DIRECTORIES(${BOOST_LIB_DIRS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SUBDIRS(common train web test)



