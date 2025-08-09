/*
    实现微服务对 MySQL 数据库操作句柄获取的代码
*/

#pragma once

// C++
#include <memory>
// odb
#include <odb/mysql/database.hxx>
#include <elasticlient/client.h>

namespace stellar_post
{
    namespace Operator
    {
        // 获取 odb 数据库操作句柄
        class ODBFactory
        {
        public:
            static auto Create(const std::string &user,
                               const std::string &pswd,
                               const std::string &host,
                               const std::string &db,
                               const std::string &cset,
                               int port,
                               int connPoolCount)
                -> std::shared_ptr<odb::core::database>
            {
                std::unique_ptr<odb::mysql::connection_pool_factory> cpf =
                    std::make_unique<odb::mysql::connection_pool_factory>(connPoolCount, 0);
                std::shared_ptr<odb::mysql::database> res =
                    std::make_shared<odb::mysql::database>(user, pswd, db, host, port, "", cset, 0, std::move(cpf));
                return res;
            }
        };

        // 获取 elasticsearch 客户端操作句柄
        class ESClientFactory
        {
        public:
            // 获取 elasticsearch 客户端操作句柄
            static auto Create(const std::vector<std::string> &hostList) -> std::shared_ptr<elasticlient::Client>
            {
                return std::make_shared<elasticlient::Client>(hostList);
            }
        };
    }
}