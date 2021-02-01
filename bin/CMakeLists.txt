set(bin md5server)

MESSAGE(STATUS "generate make for ${bin}")
add_executable(${bin} main.cpp)
add_dependencies(${bin} md5calc queue threadpool)
target_link_libraries(${bin} PRIVATE md5calc_LIBRARIES)
target_link_libraries(${bin} PRIVATE queue_LIBRARIES)
target_link_libraries(${bin} PRIVATE threadpool_LIBRARIES)
