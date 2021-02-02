
SET(MODULES core md5calc queue threadpool)
foreach(module ${MODULES})
    MESSAGE(STATUS "generate module ${module}")
    add_subdirectory(${module})
endforeach()
