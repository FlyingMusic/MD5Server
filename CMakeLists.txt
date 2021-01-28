cmake_minimum_required(VERSION 2.6)

# Set the project name
project (server)

SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -fPIC -g -Wno-deprecated -fpermissive -ldl -lrt -std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -fPIC -O2 -Wall -Wno-deprecated -fpermissive -ldl -lrt -std=c++11")

SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -fPIC -g -Wno-deprecated -fpermissive -ldl -lrt")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -fPIC-O2 -Wall -Wno-deprecated -fpermissive -ldl -lrt")

SET(PLATFORM_DEPS "-lpthread -lm")

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
	server.cpp
	ThreadDeque.cpp
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
