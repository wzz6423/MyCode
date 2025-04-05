.PHONY:all
all:tcpserverd tcpclient

tcpserverd:Main.cpp
	@g++ -o $@ $^ std=c++11

tcpclient:TcpClient.cpp
	@g++ -o $@ $^ std=c++11

.PHONY:clean
clean:
	@rm -f tcpserverd tcpclient