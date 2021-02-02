project(threadpool)

add_library(${PROJECT_NAME} STATIC ThreadPool.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

#add_subdirectory(bin)
