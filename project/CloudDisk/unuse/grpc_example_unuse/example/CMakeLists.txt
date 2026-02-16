cmake_minimum_required(VERSION 3.18)
project(CalculatorGrpc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 统一使用CONFIG模式查找Protobuf和gRPC
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

message(STATUS "Protobuf found: v${Protobuf_VERSION}")
message(STATUS "gRPC found: v${gRPC_VERSION}")

# 获取Protobuf和gRPC的可执行工具路径
get_target_property(Protobuf_PROTOC_EXECUTABLE protobuf::protoc IMPORTED_LOCATION_RELEASE)
get_target_property(GRPC_CPP_PLUGIN gRPC::grpc_cpp_plugin IMPORTED_LOCATION_RELEASE)

message(STATUS "Protoc executable: ${Protobuf_PROTOC_EXECUTABLE}")
message(STATUS "gRPC plugin: ${GRPC_CPP_PLUGIN}")

# 设置proto文件
set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cal.proto)
get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)

# 生成Proto文件
set(GENERATED_PROTO_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_PROTO_DIR})

set(PROTO_SRCS
        ${GENERATED_PROTO_DIR}/${PROTO_NAME}.pb.cc
        ${GENERATED_PROTO_DIR}/${PROTO_NAME}.pb.h
        ${GENERATED_PROTO_DIR}/${PROTO_NAME}.grpc.pb.cc
        ${GENERATED_PROTO_DIR}/${PROTO_NAME}.grpc.pb.h
)

# 添加protoc生成命令
add_custom_command(
        OUTPUT ${PROTO_SRCS}
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        --cpp_out=${GENERATED_PROTO_DIR}
        --grpc_out=${GENERATED_PROTO_DIR}
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
        ${PROTO_FILE}
        DEPENDS ${PROTO_FILE}
        COMMENT "Generating gRPC code from cal.proto"
)

# 添加可执行文件
add_executable(server server.cpp ${PROTO_SRCS})
add_executable(client client.cpp ${PROTO_SRCS})

# 包含生成的proto目录
target_include_directories(server PRIVATE ${GENERATED_PROTO_DIR})
target_include_directories(client PRIVATE ${GENERATED_PROTO_DIR})

# 简化链接 - gRPC核心目标自动包含所有依赖
target_link_libraries(server PRIVATE
        gRPC::grpc++
        gRPC::grpc
        protobuf::libprotobuf
)

target_link_libraries(client PRIVATE
        gRPC::grpc++
        gRPC::grpc
        protobuf::libprotobuf
)

# 添加系统库（平台无关）
if(UNIX)
    target_link_libraries(server PRIVATE Threads::Threads)
    target_link_libraries(client PRIVATE Threads::Threads)

    find_library(MATH_LIB m)
    if(MATH_LIB)
        target_link_libraries(server PRIVATE m)
        target_link_libraries(client PRIVATE m)
    endif()
endif()

# 启用必需的C++特性
target_compile_features(server PRIVATE cxx_std_17)
target_compile_features(client PRIVATE cxx_std_17)

# 设置安装规则（可选）
install(TARGETS server client RUNTIME DESTINATION bin)