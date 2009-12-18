PROJECT(flightpred_common)
SET(CMAKE_MODULE_PATH ${flightpred_common_SOURCE_DIR})

SET(libsources 
features_weather.cpp
forecast.cpp
geo_parser.cpp
grab_flights.cpp
grab_grib.cpp
grib_pred_model.cpp
solution_config.cpp
solution_manager.cpp
train_svm.cpp
reporter.cpp
)

ADD_LIBRARY(flightpred_common ${libsources})
TARGET_LINK_LIBRARIES(flightpred_common ${BOOST_WT_LIBRARIES} )

SET_TARGET_PROPERTIES(
    flightpred_common
  PROPERTIES
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    SOVERSION ${flightpred_common_SOVERSION}
    DEBUG_POSTFIX "d"
  )

#INSTALL(TARGETS flightpred_common
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
#    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

