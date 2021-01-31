set(BINS md5_test)

foreach(bin ${BINS})
    MESSAGE(STATUS "generate make for ${bin}")
    add_executable(${bin} ${bin}.cpp)
    target_link_libraries(${bin} PRIVATE ${module}_LIBRARIES)
endforeach()

