
SET(MODULES queue)
foreach(module ${MODULES})
    MESSAGE(STATUS "generate module ${module}")
    include_directories(${module})
    set(${module}_LIBRARIES "${BUILD_PATH}/${module}/lib${module}.a")
    add_subdirectory(${module})
    MESSAGE(STATUS "${module} Library:${${module}_LIBRARIES}")
endforeach()
