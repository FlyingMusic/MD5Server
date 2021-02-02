set(bin md5server)

MESSAGE(STATUS "generate make for ${bin}")
add_executable(${bin} main.cpp)
target_link_libraries(${bin} core md5calc queue threadpool pthread)
add_dependencies(${bin} core md5calc queue threadpool)
