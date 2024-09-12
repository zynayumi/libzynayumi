cmake_minimum_required (VERSION 3.0)
project (LibZynayumi)

# Print pretty summary
include(Summary.cmake)

find_package (Boost 1.54 REQUIRED)

# uncomment to be in Release mode [default]
# set(CMAKE_BUILD_TYPE Release)

# uncomment to build in debug mode
# set(CMAKE_BUILD_TYPE Debug)

# default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE Release)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "-fPIC -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g0")

if(Boost_FOUND)
  set(HAVE_LIBZYNAYUMI 1)
endif(Boost_FOUND)

add_subdirectory(src)

summary_add("LibZynayumi" "Library of Zynayumi" HAVE_LIBZYNAYUMI)
summary_show()
