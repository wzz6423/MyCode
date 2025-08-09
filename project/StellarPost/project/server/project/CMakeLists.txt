# 添加 cmake 版本
cmake_minimum_required(VERSION 3.10)

project(StellarPost)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/speech)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/file)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/user)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/transmite)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/message)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/crony)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/gateway)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})