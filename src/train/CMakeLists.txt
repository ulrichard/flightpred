PROJECT(flightpred_train)
SET(CMAKE_MODULE_PATH ${flightpred_train_SOURCE_DIR})

ADD_EXECUTABLE(flightpred_train area_mgr.cpp main.cpp)
TARGET_LINK_LIBRARIES(flightpred_train flightpred_common boost_program_options-mt boost_filesystem-mt boost_date_time-mt boost_system-mt boost_thread-mt boost_regex-mt boost_serialization-mt boost_iostreams-mt jsonspirit pqxx grib_api evocosm coyotl)

SET_TARGET_PROPERTIES(
    flightpred_train
  PROPERTIES
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    SOVERSION ${flightpred_train_SOVERSION}
    DEBUG_POSTFIX "d"
  )

INSTALL(TARGETS flightpred_train
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
