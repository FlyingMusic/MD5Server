set(module md5calc)

add_library(${module}_LIBRARIES STATIC
    md5.cpp
    )

add_subdirectory(bin)
