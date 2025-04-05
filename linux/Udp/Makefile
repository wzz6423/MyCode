.PHONY:all
all:UdpClient UdpServer

UdpClient:UdpClient.cpp
	@g++ -o $@ $^ -std=c++11
UdpServer:Main.cpp
	@g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	@rm -f UdpClient UdpServer