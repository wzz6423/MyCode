# jsoncpp 安装教程

```bash
# 命令行
sudo apt update

sudo apt install libjsoncpp-dev

# 源码安装
sudo apt install cmake git build-essential

git clone https://github.com/open-source-parsers/jsoncpp.git

cd jsoncpp

mkdir build

cd build

cmake -DCMAKE_BUILD_TYPE=release -DBUILD_SHARED_LIBS=ON ..

make -j$(nproc) # 并行编译（nproc为CPU核心数）

sudo make install

```