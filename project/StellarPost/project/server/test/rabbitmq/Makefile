all:consume publish

consume:consume.cpp
	@g++ -o $@ $^ -std=c++23 -lamqpcpp -lev -lfmt -lspdlog -lgflags

publish:publish.cpp
	@g++ -o $@ $^ -std=c++23 -lamqpcpp -lev -lfmt -lspdlog -lgflags

.PHONY:clean
clean:
	@rm -f consume publish