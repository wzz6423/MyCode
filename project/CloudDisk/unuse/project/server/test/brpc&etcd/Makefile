all:registry discovery

registry:registry.cpp brpc.pb.cc
	@g++ -o $@ $^ -std=c++23 -letcd-cpp-api -lcpprest -lspdlog  -lpthread -lgflags -lbrpc -lssl -lcrypto -lprotobuf -lpthread -lleveldb

discovery:discovery.cpp brpc.pb.cc
	@g++ -o $@ $^ -std=c++23 -letcd-cpp-api -lcpprest -lspdlog  -lpthread -lgflags -lbrpc -lssl -lcrypto -lprotobuf -lpthread -lleveldb

.PHONY:clean
clean:
	@rm -f registry discovery