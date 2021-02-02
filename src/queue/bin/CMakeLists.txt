set(BINS queue_test thread_test)

if(DEBUG_CONCURRENT_QUEUE)
    add_definitions(-D_DEBUG_CONCURRENT_QUEUE_)
endif(DEBUG_CONCURRENT_QUEUE)

foreach(bin ${BINS})
    MESSAGE(STATUS "generate make for ${bin}")
    add_executable(${bin} ${bin}.cpp)
    target_link_libraries(${bin} queue)
    target_link_libraries(${bin} pthread)
endforeach()

