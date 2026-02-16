# grpc 安装教程

```bash
# 安装依赖
sudo apt update
sudo apt install -y build-essential autoconf libtool pkg-config cmake clang libc++-dev libc++abi-dev git

# 命令行安装
sudo apt-get install -y git g++ make libssl-dev libprotobuf-dev libprotoc-dev protobuf-compiler libleveldb-dev
# ===========================================================================================================
# 下载 protobuf 源码
git clone https://github.com/protocolbuffers/protobuf.git
cd protobuf
git submodule update --init --recursive

# 编译安装
# ------------------------------------------------------------------------------------------------------------
./autogen.sh # 有这个文件采取这个方式
./configure
# ------------------------------------------------------------------------------------------------------------
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
sudo ldconfig
# ===========================================================================================================

# 下载源码（包含子模块）
git clone --recurse-submodules https://github.com/grpc/grpc
cd grpc
# -------------------------------------------------------------------------------------------------
# 或者
git clone https://github.com/grpc/grpc
cd grpc
git submodule update --init
# =================================================================================================
# 如果网络问题可以尝试: 
# 1. 生成 SSH 密钥（如果尚未生成）
# 生成 SSH 密钥（如果尚未生成）
ssh-keygen -t ed25519 -C "your_email@example.com"

# 将公钥添加到 ssh-agent
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519

# 复制公钥内容（粘贴到 GitHub 的 SSH Keys 设置中）
cat ~/.ssh/id_ed25519.pub
# 复制显示的全部内容
# 将公钥添加到GitHub​​：
# 登录GitHub → Settings → SSH and GPG keys → New SSH key
# 填写Title，Key粘贴复制的公钥
# 点击Add SSH key

git clone git@github.com:grpc/grpc.git
cd grpc
# 1. 重置所有子模块
git submodule deinit -f .
rm -rf .git/modules

# 2. 更新git配置
git config --global http.retry 5
git config --global http.lowSpeedTime 300000

# 3. 修改为SSH协议（关键步骤）
sed -i 's|https://github.com/|git@github.com:|' .gitmodules
git submodule sync

git submodule update --init

# =================================================================================================
# 不需要密钥了, 删除: (可选)
rm -v ~/.ssh/id_ed25519 ~/.ssh/id_ed25519.pub
ssh-add -d ~/.ssh/id_ed25519

# 方法一：精准删除 GitHub 条目
ssh-keygen -R github.com
# 方法二：手动编辑文件
nano ~/.ssh/known_hosts  # 删除含 "github.com" 的行

# 在 GitHub 上删除密钥
# -------------------------------------------------------------------------------------------------
mkdir -p cmake/build
cd cmake/build

# 编译安装
cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local ../..
make -j$(nproc)
sudo make install

```
