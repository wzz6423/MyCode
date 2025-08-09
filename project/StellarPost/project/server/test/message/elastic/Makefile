test:test.cpp
	@c++ -o $@ $^ -std=c++23 -lgflags -lspdlog -lfmt -lpthread -lcpr -lelasticlient /usr/lib/x86_64-linux-gnu/libjsoncpp.so.19

.PHONY:clean
clean:
	@rm -f test