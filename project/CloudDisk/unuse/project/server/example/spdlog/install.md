# spdlog 安装教程

```bash
# 命令安装
sudo apt-get install libspdlog-dev

# 源码安装
git clone https://github.com/gabime/spdlog.git

cd spdlog

mkdir build

cd build

cmake -DCMAKE_INSTALL_PREFIX=/usr ..

make

sudo make install

```