elastic:elastic.cpp
	@g++ -o $@ $^ -lspdlog -lfmt -lpthread -lcpr -lelasticlient -ljsoncpp -std=c++23

.PHONY:clean
clean:
	@rm -f elastic