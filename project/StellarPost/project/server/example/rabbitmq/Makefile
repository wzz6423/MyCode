all:consume publish

consume:consume.cpp
	@g++ -o $@ $^ -std=c++23 -lamqpcpp -lev

publish:publish.cpp
	@g++ -o $@ $^ -std=c++23 -lamqpcpp -lev

.PHONY:clean
clean:
	@rm -f consume publish