PROJECT(flightpred_train)
SET(CMAKE_MODULE_PATH ${flightpred_train_SOURCE_DIR})

ADD_EXECUTABLE(flightpred_train area_mgr.cpp main.cpp)
TARGET_LINK_LIBRARIES(flightpred_train 
	flightpred_common
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_DATE_TIME_LIBRARY}
	${Boost_THREAD_LIBRARY}
	${Boost_REGEX_LIBRARY}
	${Boost_SERIALIZATION_LIBRARY}
	${Boost_IOSTREAMS_LIBRARY}
	jsonspirit
	pqxx 
	grib_api 
	evocosm 
	coyotl
)

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
