.PHONY:all
all:processa processb

processa:processa.cxx
	@g++ -o $@ $^ -std=c++11
processb:processb.cxx
	@g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	@rm -f processa processb