project(md5calc)

add_library(${PROJECT_NAME} STATIC md5.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

add_subdirectory(bin)
