all: client server

client:brpc.pb.cc client.cpp
	@g++ -o $@ $^ -std=c++23 -lbrpc -lprotobuf -lpthread -lleveldb -lgflags -lssl -lcrypto

server:brpc.pb.cc server.cpp
	@g++ -o $@ $^ -std=c++23 -lbrpc -lprotobuf -lpthread -lleveldb -lgflags -lssl -lcrypto

.PHONY: clean
clean:
	@rm -f client server