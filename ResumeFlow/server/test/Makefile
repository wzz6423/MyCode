# UnitTest:UnitTest.cpp ../project/bundle/bundle.cpp
UnitTest:UnitTest.cpp
	@g++ -o $@ $^ -std=c++23 -L../project/bundle/lib/ -lpthread -lstdc++fs -ljsoncpp -lbundle

.PHONY:clean
clean:
	@rm -f UnitTest