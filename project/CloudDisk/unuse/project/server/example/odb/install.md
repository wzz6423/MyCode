# odb 安装教程

```bash
# 安装 build2
# build2 安装时可能会版本更新, 先看官网安装步骤: https://build2.org/install.xhtml#unix
curl -sSfO https://download.build2.org/0.17.0/build2-install-0.17.0.sh

sh build2-install-0.17.0.sh
# 如果因为超时导致失败可以将超时时间设置得更长: sh build2-install-0.17.0.sh --timeout 1800

# 安装 odb-compiler
# 此处gcc版本根据你机子 gcc 版本及其支持的 gcc 版本而定
sudo apt-get install gcc-11-plugin-dev

mkdir odb-build && cd odb-build

bpkg create -d odb-gcc-N cc config.cxx=g++ config.cc.coptions=-O3 config.bin.rpath=/usr/lib config.install.root=/usr/ config.install.sudo=sudo

cd odb-gcc-N

bpkg build odb@https://pkg.cppget.org/1/beta

bpkg test odb test odb-2.5.0-b.25+1/tests/testscript{testscript} tested odb/2.5.0-b.25+1

bpkg install odb

odb --version
# 如果报错: No such file or directory, 执行下面的部分
# ---------------------------------------------------------------------------------------------------------------
sudo echo 'export PATH=${PATH}:/usr/local/bin' >> ~/.bashrc

export PATH=${PATH}:/usr/local/bin

odb --version
# ---------------------------------------------------------------------------------------------------------------

# 安装 odb 运行时库
cd ..

bpkg create -d libodb-gcc-N cc config.cxx=g++ config.cc.coptions=-O3 config.install.root=/usr/ config.install.sudo=sudo

cd libodb-gcc-N

bpkg add https://pkg.cppget.org/1/beta

bpkg fetch

bpkg build libodb

bpkg build libodb-mysql

# 安装 mysql 和客户端开发包
sudo apt install mysql-server

sudo apt install -y libmysqlclient-dev

# 配置 mysql
# ---------------------------------------------------------------------------------------------------------------
sudo vim /etc/my.cnf 或者 /etc/mysql/my.cnf 有哪个修改哪个就行
#添加以下内容
[client]
default-character-set=utf8
[mysql]
default-character-set=utf8
[mysqld]
character-set-server=utf8
bind-address = 0.0.0.0

# 修改 root 密码
bash : sudo cat /etc/mysql/debian.cnf

# Automatically generated for Debian scripts. DO NOT TOUCH!
[client]
host = localhost
user = debian-sys-maint
 **password = UWcn9vY0NkrbJMRC** 
socket = /var/run/mysqld/mysqld.sock
[mysql_upgrade]
host = localhost
user = debian-sys-maint
password = UWcn9vY0NkrbJMRC
socket = /var/run/mysqld/mysqld.sock

bash : sudo mysql -u debian-sys-maint -p

Enter password: #这里输入上边看到的密码

mysql> ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'xxxxxx';
Query OK, 0 rows affected (0.01 sec) # mysql 提示的不是自己输入的

mysql> FLUSH PRIVILEGES;
Query OK, 0 rows affected (0.01 sec)# mysql 提示的不是自己输入的

mysql> quit

# ---------------------------------------------------------------------------------------------------------------

sudo systemctl restart mysql
sudo systemctl enable mysql

# 安装 boost profile 库
bpkg build libodb-boost

# 打包安装
bpkg install --all --recursive

# 建议自行给 mysql 添加一个普通账户并赋予权限, 用 root 不太好

```