PROJECT(flightpred_web)
SET(CMAKE_MODULE_PATH ${flightpred_train_SOURCE_DIR})

ADD_EXECUTABLE(flightpred_web.fcgi main.cpp )
TARGET_LINK_LIBRARIES(flightpred_web.fcgi flightpred_common wt wtfcgi wtext pqxx grib_api )

SET_TARGET_PROPERTIES(
    flightpred_web.fcgi
  PROPERTIES
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    SOVERSION ${flightpred_web_SOVERSION}
    DEBUG_POSTFIX "d"
  )

INSTALL(TARGETS flightpred_web.fcgi
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

