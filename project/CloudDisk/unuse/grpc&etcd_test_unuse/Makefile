all:discovery registry

CXX = g++
CXXFLAGS = -std=c++23 -I/usr/local/include -I.
LDFLAGS = -L/usr/local/lib

# grpc 需要链接的库很多, 没法一个个写
# 使用 pkg-config 获取动态链接参数
GRPC_LIBS = $(shell pkg-config --libs --shared grpc++ grpc protobuf absl_base absl_log absl_status absl_strings)

discovery:discovery.cpp grpc.grpc.pb.cc grpc.pb.cc
	@$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(GRPC_LIBS) -letcd-cpp-api -lcpprest -lspdlog -lfmt -lpthread -lgflags

registry:registry.cpp grpc.grpc.pb.cc grpc.pb.cc
	@ $(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(GRPC_LIBS) -letcd-cpp-api -lcpprest -lspdlog -lfmt -lpthread -lgflags

.PHONY:clean
clean:
	@rm -f discovery registry *pb*
