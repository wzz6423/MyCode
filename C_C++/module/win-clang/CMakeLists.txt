cmake_minimum_required(VERSION 3.28)  # 必须使用CMake 3.26或更高版本，因为模块支持从3.26开始
project(module)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件，并声明模块接口文件
add_executable(module)

# 添加主文件
target_sources(module PRIVATE main.cpp)

# 添加模块实现文件（私有）
target_sources(module PRIVATE MyMath.cpp)

# 声明模块接口文件（使用FILE_SET）
target_sources(module
               PUBLIC
               FILE_SET CXX_MODULES  # 文件集类型为CXX_MODULES
               TYPE CXX_MODULES      # 类型为CXX_MODULES
               FILES MyMath.ixx      # 模块接口文件
)