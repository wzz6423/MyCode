# brpc 安装教程

```bash
# 安装依赖
sudo apt-get install -y git g++ make libssl-dev libprotobuf-dev libprotoc-dev protobuf-compiler libleveldb-dev

git clone https://github.com/apache/brpc.git
cd brpc/
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr .. && cmake --build . -j6
make && sudo make install

```