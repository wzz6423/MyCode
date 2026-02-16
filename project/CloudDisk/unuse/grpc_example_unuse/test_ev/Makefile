# 静态库
#all:server client
#
#CXX = g++
#CXXFLAGS = -std=c++23 -I/usr/local/include -I.
#LDFLAGS = -L/usr/local/lib
#
## grpc 需要链接的库很多, 没法一个个写
## 使用 pkg-config 获取静态链接参数
#GRPC_LIBS = $(shell pkg-config --libs --static grpc++ grpc protobuf absl_base absl_log absl_status absl_strings)
#
#server:server.cpp example.grpc.pb.cc example.pb.cc
#	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(GRPC_LIBS)
#
#client:client.cpp example.grpc.pb.cc example.pb.cc
#	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(GRPC_LIBS)
#
#.PHONY:clean
#clean:
#	@rm -f server client

# 动态库
all:server client

CXX = g++
CXXFLAGS = -std=c++23 -I/usr/local/include -I.
LDFLAGS = -L/usr/local/lib

# grpc 需要链接的库很多, 没法一个个写
# 使用 pkg-config 获取动态链接参数
GRPC_LIBS = $(shell pkg-config --libs --shared grpc++ grpc protobuf absl_base absl_log absl_status absl_strings)

server:server.cpp example.grpc.pb.cc example.pb.cc
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(GRPC_LIBS)

client:client.cpp example.grpc.pb.cc example.pb.cc
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(GRPC_LIBS)

.PHONY:clean
clean:
	@rm -f server client