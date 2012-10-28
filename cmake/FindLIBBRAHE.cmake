#=============================================================================
# - Find libbrahe libraries
# This module finds if libbrahe is installed and determines where the
# include files and libraries are.
# This code sets the following variables:
#
#  LIBBRAHE_FOUND         - have the libbrahe libs been found
#  LIBBRAHE_INCLUDE_DIRS  - directories where the libbrahe headers can be found found
#  LIBBRAHE_LIBRARIES     - path to the CrashReport import libraries
#=============================================================================
# Copyright 2012 Richard Ulrich
#=============================================================================

FIND_PATH(LIBBRAHE_INCLUDE_DIR libbrahe/prng.h HINTS include $ENV{INCLUDE})

FIND_LIBRARY(LIBBRAHE_LIBRARY  NAMES brahe PATHS PATH /usr /usr/local PATH_SUFFIXES lib)

SET(LIBBRAHE_INCLUDE_DIRS ${LIBBRAHE_INCLUDE_DIR})
SET(LIBBRAHE_LIBRARIES    ${LIBBRAHE_LIBRARY})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBBRAHE DEFAULT_MSG
	LIBBRAHE_INCLUDE_DIR
	LIBBRAHE_LIBRARY
)

MARK_AS_ADVANCED(
	LIBBRAHE_INCLUDE_DIR
	LIBBRAHE_LIBRARY
	LIBBRAHE_INCLUDE_DIRS
	LIBBRAHE_LIBRARIES
)


