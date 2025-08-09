test:test.cpp
	@c++ -o $@ $^ -std=c++23 -lgflags -lgflags -lhiredis -lredis++ -lpthread
	
.PHONY:clean
clean:
	@rm -f test