cmake_minimum_required (VERSION 0.1)
project (Zynayumi)

# Print pretty summary
include(Summary.cmake)

find_package (Boost 1.54 REQUIRED)
find_package (PkgConfig 0.16 REQUIRED)
pkg_check_modules(DSSI dssi>=1.0)

# uncomment to be in Release mode [default]
# set(CMAKE_BUILD_TYPE Release)

# uncomment to build in debug mode
# set(CMAKE_BUILD_TYPE Debug)

# default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE Release)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "")

set(CMAKE_CXX_FLAGS "-std=gnu++0x -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g0")

if(Boost_FOUND)
  set(HAVE_ZYNAYUMI 1)
endif(Boost_FOUND)

if(HAVE_ZYNAYUMI AND DSSI_FOUND)
  set(HAVE_DSSI 1)
endif(HAVE_ZYNAYUMI AND DSSI_FOUND)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VST3 SDK/public.sdk/source/vst2.x/audioeffectx.h")
  set(HAVE_VST 1)
endif()

add_subdirectory(src)

summary_add("Zynayumi" "Library of Zynayumi" HAVE_ZYNAYUMI)
summary_add("Zynayumi-DSSI" "DSSI plugin of Zynayumi" HAVE_DSSI)
summary_add("Zynayumi-VST" "VST plugin of Zynayumi" HAVE_VST)
summary_show()
