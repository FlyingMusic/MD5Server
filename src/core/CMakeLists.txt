set(module core)

add_library(${module}_LIBRARIES STATIC
    io_callback.cpp
    work_callback.cpp
    )
