PROJECT(flightpred_web)
SET(CMAKE_MODULE_PATH ${flightpred_web_SOURCE_DIR})

FIND_PACKAGE(Wt 3.0 REQUIRED)

include_directories(${Wt_INCLUDE_DIR})

SET(FLP_WEB_SRC
    main.cpp
    FlightForecast.cpp
    WeatherMap.cpp
    Populations.cpp
)

SET(FLP_WEB_LIB
    flightpred_common
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_REGEX_LIBRARY}
	${Boost_SIGNALS_LIBRARY}
	${Boost_DATE_TIME_LIBRARY}
 	wt
	wtext
	pqxx
	grib_api
)

ADD_EXECUTABLE(flightpred_web.fcgi ${FLP_WEB_SRC})
TARGET_LINK_LIBRARIES(flightpred_web.fcgi ${FLP_WEB_LIB} wtfcgi)
SET_TARGET_PROPERTIES(flightpred_web.fcgi PROPERTIES
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    SOVERSION ${flightpred_web_SOVERSION}
    DEBUG_POSTFIX "d"
)

ADD_EXECUTABLE(flightpred_http ${FLP_WEB_SRC})
TARGET_LINK_LIBRARIES(flightpred_http ${FLP_WEB_LIB} wthttp)
SET_TARGET_PROPERTIES(flightpred_http PROPERTIES
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    SOVERSION ${flightpred_web_SOVERSION}
    DEBUG_POSTFIX "d"
)

IF(NOT EXISTS /etc/flightpred/wt_config.xml)
	INSTALL(FILES ${flightpred_MAIN_DIR}/scripts/wt_config.xml DESTINATION /etc/flightpred/)
ENDIF (NOT EXISTS /etc/flightpred/wt_config.xml)
INSTALL(FILES ${flightpred_MAIN_DIR}/scripts/flightpred.local          DESTINATION /etc/apache2/sites-available/)
INSTALL(FILES ${flightpred_MAIN_DIR}/scripts/flightpred.homelinux.org  DESTINATION /etc/apache2/sites-available/)
INSTALL(FILES ${flightpred_MAIN_DIR}/scripts/flightpred.ulrichard.ch   DESTINATION /etc/apache2/sites-available/)

INSTALL(DIRECTORY ${flightpred_MAIN_DIR}/www_docroot DESTINATION ${CMAKE_INSTALL_PREFIX}/share/flightpred/)
INSTALL(DIRECTORY /usr/share/Wt/resources            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/flightpred/www_docroot/)

INSTALL(TARGETS flightpred_web.fcgi                  RUNTIME DESTINATION share/flightpred/www_docroot)

