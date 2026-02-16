// 测试通过

// C++
#include <iostream>
#include <vector>
// es
#include "../../project/common/elasticsearch/elasticsearch.hpp"

auto main() -> int
{
    std::vector<std::string> hostList = {"http://127.0.0.1:9200/"};
    std::shared_ptr<elasticlient::Client> client = std::make_shared<elasticlient::Client>(hostList);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // 构建索引测试
    // 1.
    // cloud_disk::ElasticSearch::EsIndex index("test", "_doc", client);
    // index.Add("nickName");
    // index.Add("phoneNumber", "keyword", "standard", true);
    // index.Create();

    // 2.
    // cloud_disk::ElasticSearch::EsIndex index("test", "_doc", client);
    // index.Add("nickName").Add("phoneNumber", "keyword", "standard", true).Create();

    // 3.
    // cloud_disk::ElasticSearch::EsIndex("test", "_doc", client).Add("nickName").Add("phoneNumber", "keyword", "standard", true).Create();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // 构建索引 & 数据插入, 检索, 删除测试
    cloud_disk::ElasticSearch::EsIndex index("test", client);
    index.Add("nickName");
    index.Add("phoneNumber", "keyword", "standard", true);
    bool ret = index.Create();
    if (ret)
    {
        cloud_disk::Log::lg("debug", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "index create success!~");
    }

    // 新增
    ret = cloud_disk::ElasticSearch::EsInsert("test", client)
              .Add("nickName", "张三")
              .Add("phoneNumber", "1234567")
              .Insert("01");
    if (ret == false)
    {
        cloud_disk::Log::lg("error", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "add data error!");
    }

    // 修改
    ret = cloud_disk::ElasticSearch::EsInsert("test", client)
              .Add("nickName", "张三")
              .Add("phoneNumber", "7654321")
              .Insert("01");
    if (ret == false)
    {
        cloud_disk::Log::lg("error", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "update data error!");
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 查找
    Json::Value user;
    auto oUser = cloud_disk::ElasticSearch::EsSerach("test", client)
                     .AddShouldMatch("phoneNumber.keyword", "7654321")
                     .Search();
    if (!oUser)
    {
        cloud_disk::Log::lg("error", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "search data error!");
    }
    else
    {
        user = oUser.value();
        if (user.empty() || user.isArray() == false)
        {
            cloud_disk::Log::lg("error", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "search data error! empty or not arr!");
        }
    }
    int sz = user.size();
    std::cout << "size: " << sz << std::endl;
    for (int i = 0; i < sz; ++i)
    {
        std::cout << user[i]["_source"]["nickName"].asString() << std::endl;
        std::cout << user[i]["_source"]["phoneNumber"].asString() << std::endl;
    }

    // 删除
    ret = cloud_disk::ElasticSearch::EsRemove("test", client)
              .Remove("01");
    if (ret == false)
    {
        cloud_disk::Log::lg("error", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "remove data error!");
    }

    return 0;
}