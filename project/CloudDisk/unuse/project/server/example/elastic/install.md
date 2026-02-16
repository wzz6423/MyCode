# elastic search 安装教程

```bash
# 安装依赖工具
sudo apt-get install apt-transport-https ca-certificates wget gnupg

# 导入 Elastic GPG 密钥
wget -qO- https://artifacts.elastic.co/GPG-KEY-elasticsearch | sudo gpg --dearmor -o /usr/share/keyrings/elastic.gpg

# 添加 Elastic 7.x 仓库（适用于 7.17.x 版本）
echo "deb [signed-by=/usr/share/keyrings/elastic.gpg] https://artifacts.elastic.co/packages/7.x/apt stable main" | sudo tee /etc/apt/sources.list.d/elastic-7.x.list
# 更新软件包列表
sudo apt update
# 安装 es
sudo apt-get install elasticsearch=7.17.21
# 启动 es
sudo systemctl start elasticsearch
# 安装 ik 分词器插件
sudo /usr/share/elasticsearch/bin/elasticsearch-plugin install https://get.infini.cloud/elasticsearch/analysis-ik/7.17.21


# 调整 es 虚拟内存, 虚拟内存默认最大映射数为 65530, 无法满足 es 系统要求, 需要调整为 262144 以上
sysctl -w vm.max_map_count=262144

```