# 生成动态库, 增量编译提高效率
OBJS = chat_session-odb.o crony_apply-odb.o crony_relation-odb.o chat_session_member-odb.o
.PHONY:test
test:test.cpp $(OBJS)
	@g++ $^ -o $@  -std=c++23 -lodb-mysql -lodb -lodb-boost -lfmt -lspdlog -lpthread -lgflags -letcd-cpp-api -lcpprest -lbrpc -lssl -lcrypto -lprotobuf -lleveldb

%.o : %.cxx
	@g++ -c $^ -o $@ -std=c++23 $(OBJS)

.PHONY:clean
clean:
	@rm -f test
	@rm -f *.o