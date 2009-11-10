PROJECT(flightpred_train)
SET(CMAKE_MODULE_PATH ${flightpred_train_SOURCE_DIR})

ADD_LIBRARY(flightpred_train area_mgr.cpp extract_features_flight.cpp train_svm.cpp main.cpp)
TARGET_LINK_LIBRARIES(flightpred_train flightpred_common ${BOOST_WT_LIBRARIES} )

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
