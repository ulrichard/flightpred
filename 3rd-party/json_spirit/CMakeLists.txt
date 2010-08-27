CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(VERSION_SERIES 4)
SET(VERSION_MAJOR 2)
SET(VERSION_MINOR 1)

PROJECT(libjsonspirit)
SUBDIRS(json_spirit json_demo json_map_demo json_test)
INCLUDE_DIRECTORIES(json_spirit)
