CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(flightpred)
SET(CMAKE_MODULE_PATH ${flightpred_SOURCE_DIR})
SET(flightpred_MAIN_DIR ${flightpred_SOURCE_DIR})

SET(VERSION_SERIES 0)
SET(VERSION_MAJOR 0)
SET(VERSION_MINOR 34)

SET(flightpred_common_SOVERSION 3)
SET(flightpred_train_SOVERSION 3)
SET(flightpred_web_SOVERSION 3)


SET(USERLIB_ROOT /usr CACHE PATH "Other installation prefix for dependent libraries")
SET(LIB_INSTALL_DIR "lib" CACHE STRING "Default path for libraries within ${CMAKE_INSTALL_PREFIX}")
SET(RUNDIR "/usr/flightpred/run" CACHE PATH "Default path for wt session management (only used by FCGI connector; not relative to CMAKE_INSTALL_PREFIX)")
SET(CONFIGDIR "/etc/flightpred" CACHE STRING "Path for the configuration files")
SET(CONFIGDIRAPACHE /etc/apache2 CACHE STRING "Path for the apache2 configuration files")
SET(DEPLOYROOT /var/www/localhost/htdocs/flightpred CACHE PATH "Path to deploy into)")

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

MARK_AS_ADVANCED(CONFIGDIR)
SET(CONFIGURATION "${CONFIGDIR}/wt_config.xml" CACHE PATH "Path to the wt configuration file")

SET(WEBUSER www-data CACHE STRING "Webserver username (e.g. apache or www)")
SET(WEBGROUP www-data CACHE STRING "Webserver groupname (e.g. apache or www or users)")

# Setup output Directories
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables.")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries.")
MARK_AS_ADVANCED(CMAKE_LIBRARY_OUTPUT_DIRECTORY CMAKE_RUNTIME_OUTPUT_DIRECTORY CMAKE_ARCHIVE_OUTPUT_DIRECTORY)

SUBDIRS(src scripts)


