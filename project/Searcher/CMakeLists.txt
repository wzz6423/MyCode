# 1.添加 cmake 版本
cmake_minimum_required(VERSION 3.10)

# 2.设置 C++ 版本
set(CMAKE_CXX_STANDARD 23)

# 3.声明工程名称
project(boostSearch)
set(target_parse "parse")
set(target_search "search")

# 5.获取源码目录下的所有源码文件
set(parse_src_files ${CMAKE_CURRENT_SOURCE_DIR}/parse.cpp)
set(search_src_files ${CMAKE_CURRENT_SOURCE_DIR}/search.cpp)

# 6.声明目标及依赖
add_executable(${target_parse} ${parse_src_files})
add_executable(${target_search} ${search_src_files})

# 7.设置头文件默认搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 8.设置需要链接的库文件
target_link_libraries(${target_search} jsoncpp pthread)

# 9.设置安装路径
install(TARGETS ${target_parse} ${target_search} RUNTIME DESTINATION bin)
