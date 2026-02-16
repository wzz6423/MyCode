cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)

project(study)

add_executable(study test.cpp main.cpp)

set(current_path ${CMAKE_CURRENT_SOURCE_DIR})
include_directories("${current_path}/log")

target_link_libraries(study -lspdlog fmt pthread)