all: etcd_put etcd_get

etcd_put:etcd_put.cc
	@g++ -o $@ $^ -std=c++23 -letcd-cpp-api -lcpprest

etcd_get:etcd_get.cc
	@g++ -o $@ $^ -std=c++23 -letcd-cpp-api -lcpprest

.PHONY: clean
clean:
	@rm -f etcd_get etcd_put