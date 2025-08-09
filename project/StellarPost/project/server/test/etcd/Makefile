all:discovery registry

discovery:etcd_discovery.cpp
	@g++ -o $@ $^ -letcd-cpp-api -lcpprest -lspdlog -lfmt -lpthread -lgflags -std=c++23

registry:etcd_registry.cpp
	@g++ -o $@ $^ -letcd-cpp-api -lcpprest -lspdlog -lfmt -lpthread -lgflags -std=c++23

.PHONY: clean
clean:
	@rm -f discovery registry