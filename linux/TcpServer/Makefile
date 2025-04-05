.PHONY:all
all:servercal clientcal

Flag=-DMySelf=1
Lib=-ljsoncpp

servercal:ServerCal.cpp
	@g++ -o $@ $^ -std=c++11 $(Lib) $(Flag)

clientcal:ClientCal.cpp
	@g++ -o $@ $^ -std=c++11 $(Lib) $(Flag) -g

.PHONY:clean
clean:
	@rm -f servercal clientcal