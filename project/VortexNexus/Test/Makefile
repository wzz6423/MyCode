all:client server

server:UnitTest.cpp
	@g++ -o $@ $^ -std=c++17 -g
client:Client.cpp
	@g++ -o $@ $^ -std=c++17 -g

.PHONY:clean
clean:
	@rm -f client server