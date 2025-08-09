cmake_minimum_required(VERSION 3.10)

project(cmake-use)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake-study)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/odb-cmake)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})