# 1.添加 cmake 版本
cmake_minimum_required(VERSION 3.10)

# 2.设置 C++ 版本
set(CMAKE_CXX_STANDARD 23)

# 3.声明工程名称
project(odb-cmake-test)

# 4.检测并生成 odb 框架代码 4.1添加所需 odb 映射代码文件名称
set(odb_path ${CMAKE_CURRENT_SOURCE_DIR}/entity)
set(odb_files student.hxx)

# 4.2检测框架代码文件是否生成
set(odb_hxx "")
set(odb_cxx "")
set(odb_srcs "")
foreach(odb_file ${odb_files})

  # 4.3如果没有生成, 则预定义生成指令 -- 用于在构建项目之前先生成框架代码
  string(REPLACE ".hxx" "-odb.hxx" odb_hxx ${odb_file})
  string(REPLACE ".hxx" "-odb.cxx" odb_cxx ${odb_file})
  if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}${odb_cxx})
    add_custom_command(
      PRE_BUILD
      COMMAND odb ARGS -d mysql --std c++11 --generate-query --generate-schema
              --profile boost/date-time ${odb_path}/${odb_file}
      DEPENDS ${odb_path}/${odb_file}
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${odb_cxx}
      COMMENT "Generating ODB code for ${CMAKE_CURRENT_BINARY_DIR}/${odb_file}")
  endif()
  # 4.4将所有生成的框架源码名称保存起来
  list(APPEND odb_srcs ${CMAKE_CURRENT_BINARY_DIR}/${odb_cxx})
endforeach()

# 5.获取源码目录下的所有源码文件
set(src_files "")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source src_files)

# 6.声明目标及依赖
add_executable(odb-cmake-test ${src_files} ${odb_srcs})

# 7.设置头文件默认搜索路径
include_directories(${odb_path} ${CMAKE_CURRENT_BINARY_DIR})

# 8.设置需要链接的库文件
target_link_libraries(odb-cmake-test gflags odb-mysql odb odb-boost)

# 9.设置安装路径
INSTALL(TARGETS odb-cmake-test RUNTIME DESTINATION bin)