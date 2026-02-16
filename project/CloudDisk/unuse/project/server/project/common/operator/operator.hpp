#pragma once

// C++
#include <memory>
#include <vector>
#include <string>
// odb
#include <odb/mysql/database.hxx>
// elasticlient
#include <elasticlient/client.h>
// 日志
#include "../log/logger.hpp"

namespace cloud_disk::Operator
{
    // MySQL 数据库连接工厂类
    class MySQLFactory
    {
    public:
        /**  创建 MySQL 数据库连接
         * @param user 数据库用户名
         * @param password 数据库密码
         * @param host 数据库主机地址
         * @param database 数据库名称
         * @param charset 字符集
         * @param port 端口号
         * @param pool_size 连接池大小
         * @return 共享指针指向数据库连接对象
         * 属性说明符[[nodiscard]] -- ​​强烈建议编译器，如果调用某个函数时忽略了其返回值，则发出警告​​
         */
        [[nodiscard]] static auto CreateConnection(
            const std::string &user,
            const std::string &password,
            const std::string &host,
            const std::string &database,
            const std::string &charset = "utf8mb4",
            unsigned int port = 3306,
            unsigned int pool_size = 10)
            -> std::shared_ptr<odb::core::database>
        {
            try
            {
                // 创建连接池工厂
                auto pool_factory = std::make_unique<odb::mysql::connection_pool_factory>(
                    pool_size, // 最大连接数
                    0          // 最小连接数
                );

                // 创建 MySQL 数据库连接
                auto db = std::make_shared<odb::mysql::database>(
                    user, password, database, host, port,
                    "",      // Unix socket 路径（空表示使用 TCP/IP）
                    charset, // 字符集
                    0,       // 连接超时（0 表示默认）
                    std::move(pool_factory));

                Log::lg("info", Log::FileName(), Log::Line(),
                        "MySQL connection created for {}@{}:{}/{}",
                        user, host, port, database);

                return db;
            }
            catch (const std::exception &e)
            {
                Log::lg("critical", Log::FileName(), Log::Line(),
                        "Failed to create MySQL connection: {}", e.what());
                throw; // 重新抛出异常
            }
        }
    };

    // Elasticsearch 客户端工厂类
    class ElasticsearchFactory
    {
    public:
        /** 创建 Elasticsearch 客户端
        * @param hosts Elasticsearch 节点地址列表（格式: "host:port"）
        * @return 共享指针指向 Elasticsearch 客户端
        */
        [[nodiscard]] static auto CreateClient(
            const std::vector<std::string> &hosts)
            -> std::shared_ptr<elasticlient::Client>
        {
            try
            {
                // 验证主机列表
                if (hosts.empty())
                {
                    throw std::invalid_argument("At least one host required");
                }

                // 创建 Elasticsearch 客户端
                auto client = std::make_shared<elasticlient::Client>(hosts);

                Log::lg("info", Log::FileName(), Log::Line(),
                        "Elasticsearch client created with {} nodes", hosts.size());

                return client;
            }
            catch (const std::exception &e)
            {
                Log::lg("critical", Log::FileName(), Log::Line(),
                        "Failed to create Elasticsearch client: {}", e.what());
                throw; // 重新抛出异常
            }
        }

        /** 创建 Elasticsearch 客户端（单节点）
         * @param host 节点地址（格式: "host:port"）
         * @return 共享指针指向 Elasticsearch 客户端
         */
        [[nodiscard]] static auto CreateClient(const std::string &host) -> std::shared_ptr<elasticlient::Client>
        {
            return CreateClient(std::vector<std::string>{host});
        }
    };
}