test:test.cpp message-odb.cxx
	@c++ -o $@ $^ -std=c++23 -lodb-mysql -lodb -lodb-boost -lfmt -lspdlog -lpthread -lgflags -letcd-cpp-api -lcpprest -lbrpc -lssl -lcrypto -lprotobuf -lleveldb

.PHONY:clean
clean:
	@rm -f test