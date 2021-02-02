cmake_minimum_required(VERSION 3.0)

# Set the project name
project (server)

# Set compile version & option
#SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_BUILD_TYPE "Debug")

SET(CXXFLAGS "-fPIC -finline-functions -Wall -Winline -pipe -ffast-math -std=c++0x -ldl")
SET(CFLAGS "-fPIC -finline-functions -Wall -Winline -pipe -ffast-math -lstdc++ -ldl")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    MESSAGE(STATUS "Enable verbose log")
    ADD_DEFINITIONS(-D_VERBOSE_LOG_=1)
    SET(CFLAGS "-g ${CFLAGS}")
    SET(CXXFLAGS "-g ${CXXFLAGS}")
    option(DEBUG_CONCURRENT_QUEUE "option for debug concurrent queue" ON)
else()
    MESSAGE(STATUS "Disable verbose log")
    SET(CFLAGS "-O2 ${CFLAGS}")
    SET(CXXFLAGS "-O2 ${CXXFLAGS}")
    option(DEBUG_CONCURRENT_QUEUE "option for debug concurrent queue" OFF)
endif()

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} ${CXXFLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} ${CXXFLAGS}")

SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} ${CFLAGS}")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} ${CFLAGS}")

# global head files directory

add_subdirectory(src)
add_subdirectory(bin)
