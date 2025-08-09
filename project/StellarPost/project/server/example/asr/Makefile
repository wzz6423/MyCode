test:test.cpp
	@g++ -o $@ $^ -std=c++23 /usr/lib/x86_64-linux-gnu/libjsoncpp.so.1.8.4 -lcurl -lcrypto

.PHONY:clean
clean:
	@rm -f test