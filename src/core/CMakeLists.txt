project(core)

add_library(${PROJECT_NAME} STATIC io_callback.cpp work_callback.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} md5calc queue threadpool)
add_dependencies(${PROJECT_NAME} md5calc queue threadpool)

