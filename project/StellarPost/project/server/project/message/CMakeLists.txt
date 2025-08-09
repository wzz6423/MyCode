# 1.添加 cmake 版本
cmake_minimum_required(VERSION 3.10)

# 2.设置 C++ 版本
set(CMAKE_CXX_STANDARD 23)

# 3.声明工程名称
project(message-server)
set(target "message-server")

# for test -- 将 test/uint_test/message/message 复制到本目录下进行单元测试

# set(test_client "test_client")

# 4.检测并生成 proto 框架代码 4.1添加所需 proto 映射代码文件名称
set(proto_path ${CMAKE_CURRENT_SOURCE_DIR}/../interface)
set(proto_files message.proto base.proto user.proto file.proto)

# 4.2检测框架代码文件是否生成
set(proto_cc "")
set(proto_h "")
set(proto_srcs "")
foreach(proto_file ${proto_files})

  # 4.3如果没有生成, 则预定义生成指令 -- 用于在构建项目之前先生成框架代码
  string(REPLACE ".proto" ".pb.cc" proto_cc ${proto_file})
  string(REPLACE ".proto" ".pb.h" proto_h ${proto_file})
  if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}${proto_cc})
    add_custom_command(
      PRE_BUILD
      COMMAND protoc ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR} -I${proto_path}
              ${proto_path}/${proto_file}
      DEPENDS ${proto_path}/${proto_file}
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${proto_cc}
      COMMENT
        "Generating PROTOBUF code for ${CMAKE_CURRENT_BINARY_DIR}/${proto_file}"
    )
  endif()
  # 4.4将所有生成的框架源码名称保存起来
  list(APPEND proto_srcs ${CMAKE_CURRENT_BINARY_DIR}/${proto_cc})
endforeach()

# 4.检测并生成 odb 框架代码 4.1添加所需 odb 映射代码文件名称
set(odb_path ${CMAKE_CURRENT_SOURCE_DIR}/odb)
set(odb_files message.hxx)

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
add_executable(${target} ${src_files} ${proto_srcs} ${odb_srcs})

# for test -- 将 test/uint_test/message/message 复制到本目录下进行单元测试

# set(test_files "")
# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/message test_files)
# add_executable(${test_client} ${test_files} ${proto_srcs} ${odb_srcs})

# 7.设置头文件默认搜索路径
include_directories(
  ${proto_path}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/odb
  ${CMAKE_CURRENT_SOURCE_DIR}/source/
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/rabbitmq/
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/operator/)

# 8.设置需要链接的库文件
target_link_libraries(
  ${target}
  /usr/lib/x86_64-linux-gnu/libjsoncpp.so.19
  gflags
  curl
  crypto
  protobuf
  fmt
  spdlog
  pthread
  brpc
  ssl
  leveldb
  etcd-cpp-api
  cpprest
  odb-mysql
  odb-boost
  odb
  amqpcpp
  ev
  cpr
  elasticlient)
  
# for test -- 将 test/uint_test/message/message 复制到本目录下进行单元测试

# target_link_libraries(
#   ${test_client}
#   /usr/lib/x86_64-linux-gnu/libjsoncpp.so.19
#   gflags
#   curl
#   crypto
#   protobuf
#   fmt
#   spdlog
#   pthread
#   brpc
#   ssl
#   leveldb
#   etcd-cpp-api
#   cpprest
#   odb-mysql
#   odb-boost
#   odb
#   amqpcpp
#   ev
#   cpr
#   elasticlient)

# 9.设置安装路径
install(TARGETS ${target} RUNTIME DESTINATION bin)
