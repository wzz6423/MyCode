# etcd 安装教程

```bash
# 命令安装
sudo apt-get install etcd-server etcd-client

# 启动 etcd 服务
sudo systemctl start etcd

# 设置开机自启动
sudo systemctl enable etcd

# 安装依赖
sudo apt-get install libboost-all-dev libssl-dev
sudo apt-get install libprotobuf-dev protobuf-compiler-grpc
sudo apt-get install libgrpc-dev libgrpc++-dev
sudo apt-get install libcpprest-dev

# api 框架安装
git clone https://github.com/etcd-cpp-apiv3/etcd-cpp-apiv3.git
cd etcd-cpp-apiv3
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc) && sudo make install

```