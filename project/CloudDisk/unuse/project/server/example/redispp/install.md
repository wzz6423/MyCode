# redis 安装教程

```bash
# 命令安装
apt install redis -y

apt install libhiredis-dev

git clone https://github.com/sewenew/redis-plus-plus.git

cd redis-plus-plus

mkdir build

cd build

cmake ..

make

sudo make install

```