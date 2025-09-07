cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(moduleTest)

add_executable(moduleTest)
add_compile_options(-fmodules-ts)

target_sources(moduleTest PRIVATE
    main.cpp
    math.cpp
)

target_sources(moduleTest
  PRIVATE
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    math.cppm
)

set_target_properties(moduleTest PROPERTIES
    CXX_MODULES_DIRECTORY "${CMAKE_BINARY_DIR}/modules"
    CXX_MODULES_STD_VERSION 23
)