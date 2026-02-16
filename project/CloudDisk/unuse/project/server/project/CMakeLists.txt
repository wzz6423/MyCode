# 添加 cmake 版本
cmake_minimum_required(VERSION 3.20.0)

project(CloudDisk)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/file)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/user)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/gateway)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})