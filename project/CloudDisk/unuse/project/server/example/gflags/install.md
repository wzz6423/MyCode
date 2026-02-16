# gflags 安装教程

```bash
# 命令安装
sudo apt-get install libgflagsdev

# 源码安装
# 下载源码
git clone https://github.com/gflags/gflags.git
# 切换目录
cd gflags/
mkdir build
cd build/
# 生成 Makefile
cmake ..
# 编译代码
make
# 安装
make install

```