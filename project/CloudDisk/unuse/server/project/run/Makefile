# CloudBackup:CloudBackup.cpp ./bundle/bundle.cpp
CloudBackup:CloudBackup.cpp
	@g++ -o $@ $^ -std=c++23 -L../bundle/lib/ -lpthread -lstdc++fs -ljsoncpp -lbundle


.PHONY:clean
clean:
	@rm -f CloudBackup