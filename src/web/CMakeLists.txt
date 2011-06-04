PROJECT(flightpred_web)
SET(CMAKE_MODULE_PATH ${flightpred_web_SOURCE_DIR})

FIND_PACKAGE(Wt 3.0 REQUIRED)

include_directories(${Wt_INCLUDE_DIR})

ADD_EXECUTABLE(flightpred_web.fcgi main.cpp WGoogleMapEx.cpp FlightForecast.cpp WeatherMap.cpp Populations.cpp)
TARGET_LINK_LIBRARIES(flightpred_web.fcgi flightpred_common wt wtfcgi wtext pqxx grib_api)

SET_TARGET_PROPERTIES(
    flightpred_web.fcgi
  PROPERTIES
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    SOVERSION ${flightpred_web_SOVERSION}
    DEBUG_POSTFIX "d"
  )

INSTALL(FILES ${flightpred_MAIN_DIR}/scripts/append_wt_config.pl DESTINATION ${CMAKE_INSTALL_PREFIX}/share/flightpred)
INSTALL(FILES ${flightpred_MAIN_DIR}/scripts/wt_config_merge.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/flightpred)

INSTALL(FILES ${flightpred_MAIN_DIR}/scripts/wt_config_flightpred.xml DESTINATION /etc/flightpred/wt_config.xml)

INSTALL(FILES ${flightpred_MAIN_DIR}/scripts/flightpred.local DESTINATION /etc/apache2/sites-available)

INSTALL(DIRECTORY ${flightpred_MAIN_DIR}/www_docroot DESTINATION ${CMAKE_INSTALL_PREFIX}/share/flightpred/)
INSTALL(DIRECTORY /usr/share/Wt/resources            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/flightpred/)

INSTALL(TARGETS flightpred_web.fcgi
    RUNTIME DESTINATION share/flightpred/www_docroot)

