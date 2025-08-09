// 测试通过

// C++
#include <iostream>
#include <vector>
// es
#include "../../../project/user/elastic/elastic.hpp"

auto main() -> int
{
    std::vector<std::string> hostList = {"http://127.0.0.1:9200/"};
    std::shared_ptr<elasticlient::Client> client = std::make_shared<elasticlient::Client>(hostList);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // 构建索引测试
    // 1.
    // stellar_post::Elastic::EsIndex index("test", "_doc", client);
    // index.Add("nickName");
    // index.Add("phoneNumber", "keyword", "standard", true);
    // index.Create();

    // 2.
    // stellar_post::Elastic::EsIndex index("test", "_doc", client);
    // index.Add("nickName").Add("phoneNumber", "keyword", "standard", true).Create();

    // 3.
    // stellar_post::Elastic::EsIndex("test", "_doc", client).Add("nickName").Add("phoneNumber", "keyword", "standard", true).Create();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // 构建索引 & 数据插入, 检索, 删除测试
    stellar_post::Elastic::EsIndex index("test", client);
    index.Add("nickName");
    index.Add("phoneNumber", "keyword", "standard", true);
    bool ret = index.Create();
    if (ret)
    {
        stellar_post::Log::lg("debug", stellar_post::Log::FileName(), stellar_post::Log::Line(), "index create success!~");
    }

    // 新增
    ret = stellar_post::Elastic::EsInsert("test", client)
              .Add("nickName", "张三")
              .Add("phoneNumber", "1234567")
              .Insert("01");
    if (ret == false)
    {
        stellar_post::Log::lg("error", stellar_post::Log::FileName(), stellar_post::Log::Line(), "add data error!");
    }

    // 修改
    ret = stellar_post::Elastic::EsInsert("test", client)
              .Add("nickName", "张三")
              .Add("phoneNumber", "7654321")
              .Insert("01");
    if (ret == false)
    {
        stellar_post::Log::lg("error", stellar_post::Log::FileName(), stellar_post::Log::Line(), "update data error!");
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 查找
    Json::Value user;
    auto oUser = stellar_post::Elastic::EsSerach("test", client)
                     .AddShouldMatch("phoneNumber.keyword", "7654321")
                     .Search();
    if (!oUser)
    {
        stellar_post::Log::lg("error", stellar_post::Log::FileName(), stellar_post::Log::Line(), "search data error!");
    }
    else
    {
        user = oUser.value();
        if (user.empty() || user.isArray() == false)
        {
            stellar_post::Log::lg("error", stellar_post::Log::FileName(), stellar_post::Log::Line(), "search data error! empty or not arr!");
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
    ret = stellar_post::Elastic::EsRemove("test", client)
              .Remove("01");
    if (ret == false)
    {
        stellar_post::Log::lg("error", stellar_post::Log::FileName(), stellar_post::Log::Line(), "remove data error!");
    }

    return 0;
}