SET(JSON_SPIRIT_SRCS
json_spirit_reader.cpp
json_spirit_value.cpp
json_spirit_writer.cpp)

FIND_PACKAGE(Boost 1.37 REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

ADD_LIBRARY(jsonspirit STATIC ${JSON_SPIRIT_SRCS})


INSTALL(TARGETS jsonspirit ARCHIVE DESTINATION lib)

INSTALL(FILES json_spirit.h json_spirit_reader.h json_spirit_utils.h json_spirit_value.h json_spirit_writer.h DESTINATION include/libjsonspirit)

