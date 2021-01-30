cmake_minimum_required(VERSION 2.6)

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
else()
    MESSAGE(STATUS "Disable verbose log")
    SET(CFLAGS "-O2 ${CFLAGS}")
    SET(CXXFLAGS "-O2 ${CXXFLAGS}")
endif()

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} ${CXXFLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} ${CXXFLAGS}")

SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} ${CFLAGS}")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} ${CFLAGS}")

# global head files directory
include_directories(${module})

# subdirectory
add_subdirectory(bin)
add_subdirectory(src)

SET(MODULES md5Server)
foreach(module ${MODULES})
    MESSAGE(STATUS "generate module ${module}")
    include_directories(${module})
    include_directories(${module}/include)
    MESSAGE(STATUS "${module}/include")
    set(${module}_LIBRARIES "${BUILD_PATH}/${module}/lib${module}.a")
    add_subdirectory(${module})
    MESSAGE(STATUS "${module} Library:${${module}_LIBRARIES}")
endforeach()



SET(PLATFORM_DEPS "-lpthread -lm")

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
	server.cpp
	ThreadQueue.cpp
	ThreadPool.cpp
	Md5.cpp
	)

# Add an executable with the above sources
add_executable(server ${SOURCES})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
#include_directories("./")

target_link_libraries(server
	${PLATFORM_DEPS}
	)
