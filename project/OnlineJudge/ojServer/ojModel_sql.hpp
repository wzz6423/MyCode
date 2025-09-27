#pragma once

// C++
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <ranges>
#include <algorithm>
#include <optional>
#include <format>
// C
#include <cstdlib>
// Other
#include "../common/utils.hpp"
#include <mysql/mysql.h>

namespace model
{
    struct Question
    {
        std::string number;      // 题目编号, 唯一
        std::string title;       // 题目标题
        std::string difficulty;  // 题目难度
        std::string description; // 题目描述
        int timeLimit;           // 时间限制
        int memoryLimit;         // 内存限制
        std::string head;        // 题目预设代码, 头部提供给用户的代码
        std::string tail;        // 题目测试用例, 尾部和用户提交代码组合交给编译运行模块进行测试的代码
    };

    class Model
    {
    public:
        Model()
        {
        }
        ~Model() = default;

        // 获取所有题目列表
        auto GetAllQuestions() -> std::optional<std::vector<std::shared_ptr<Question>>>
        {
            std::string sql = std::format("select * from {}", tableName);
            return query(sql);
        }

        // 获取一个题目详细内容
        auto GetQuestion(const std::string &number) -> std::optional<std::shared_ptr<Question>>
        {
            std::string sql = std::format("select * from {} where number = {}", tableName, number);
            std::optional<std::vector<std::shared_ptr<Question>>> ret = query(sql);
            if (ret.has_value() && ret->size() == 1)
            {
                return ret->front();
            }
            return std::nullopt;
        }

    private:
        auto query(const std::string &sql) -> std::optional<std::vector<std::shared_ptr<Question>>>
        {
            // 获取 mysql 连接句柄
            MYSQL *conn = mysql_init(nullptr);
            if (conn == nullptr)
            {
                LOG(ERROR) << "mysql_init failed" << std::endl;
                return std::nullopt;
            }

            // 连接数据库
            if (mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(),
                                   dbName.c_str(), port, nullptr, 0) == nullptr)
            {
                LOG(ERROR) << "mysql_real_connect failed" << std::endl;
                return std::nullopt;
            }

            // 设置字符集
            if (mysql_set_character_set(conn, "utf8") != 0)
            {
                LOG(ERROR) << "mysql_set_character_set failed" << std::endl;
                return std::nullopt;
            }

            // 执行 sql 语句
            if (mysql_query(conn, sql.c_str()) != 0)
            {
                LOG(ERROR) << "mysql_query failed" << std::endl;
                return std::nullopt;
            }

            // 获取结果集
            MYSQL_RES *res = mysql_store_result(conn);
            if (res == nullptr)
            {
                LOG(ERROR) << "mysql_store_result failed" << std::endl;
                return std::nullopt;
            }

            // 处理结果集
            std::vector<std::shared_ptr<Question>> questions;
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                questions.push_back(std::make_shared<Question>(
                    row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]));
            }
            mysql_free_result(res);

            // 关闭连接
            mysql_close(conn);
            return questions;
        }

    private:
        inline static constexpr std::string dbName = "oj";           // 数据库名称
        inline static constexpr std::string tableName = "questions"; // 表名称
        inline static constexpr std::string user = "oj";             // 数据库用户名
        inline static constexpr std::string password = "oj";         // 数据库用户密码
        inline static constexpr std::string host = "127.0.0.1";      // 数据库服务器地址
        inline static constexpr int port = 3306;                     // 数据库服务器端口
    };
}