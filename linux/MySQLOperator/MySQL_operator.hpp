#pragma once

// C标准库
#include <cstring>
// C++标准库
#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <variant>
#include <mutex>
#include <queue>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <future>
// MySQL Connector/C++库(用于MySQL数据库交互)
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
// log
#include "log.hpp"

// 前向声明 ORM 命名空间中的 ModelTraits 模板, 避免循环依赖
namespace ORM
{
    template <typename T>
    struct ModelTraits;
}

// ====================== 数据库连接池管理 ======================
/**
 * @brief 数据库连接池类, 负责管理 MySQL 连接的创建、复用和回收
 *
 * 采用连接池模式减少频繁创建和销毁数据库连接的开销, 提高性能
 * 连接池会预先创建一定数量的连接, 当需要时分配, 使用完毕后回收
 */
class ConnectionPool
{
public:
    /**
     * @brief 构造函数, 初始化连接池并创建指定数量的连接
     * @param url 数据库连接地址(格式: tcp://ip:port)
     * @param user 数据库用户名
     * @param password 数据库密码
     * @param db 要连接的数据库名称
     * @param size 连接池初始连接数量(默认10个)
     */
    ConnectionPool(const std::string &url,
                   const std::string &user,
                   const std::string &password,
                   const std::string &db,
                   size_t size = 10)
        : url_(url), user_(user), password_(password), db_(db)
    {
        // 预先创建指定数量的连接并加入连接池
        for (size_t i = 0; i < size; ++i)
        {
            connections_.push(CreateConnection());
        }
    }

    /**
     * @brief 获取一个数据库连接
     * @return 智能指针包装的数据库连接, 自动管理生命周期
     *
     * 当连接池为空时, 会动态创建新连接; 否则复用现有连接
     * 智能指针的析构行为已被定制为自动将连接归还到连接池
     */
    auto GetConnection() -> std::shared_ptr<sql::Connection>
    {
        std::lock_guard lock(mutex_); // 加锁保证线程安全
        if (connections_.empty())
        {
            // 连接池为空时创建新连接
            return CreateConnection();
        }
        // 从连接池头部获取一个连接
        auto conn = connections_.front();
        connections_.pop();
        return conn;
    }

    /**
     * @brief 将连接归还到连接池
     * @param conn 要归还的数据库连接
     *
     * 通常不需要手动调用, 智能指针会在超出作用域时自动调用
     */
    auto ReturnConnection(std::shared_ptr<sql::Connection> conn) -> void
    {
        std::lock_guard lock(mutex_); // 加锁保证线程安全
        connections_.push(conn);      // 将连接放回队列尾部
    }

    /**
     * @brief 获取连接池当前状态
     * @return 一个 pair, 第一个元素是空闲连接数, 第二个元素是总连接数
     */
    auto Status() const -> std::pair<size_t, size_t>
    {
        std::lock_guard lock(mutex_); // 加锁保证线程安全
        return {connections_.size(), connections_.size() + borrowed_};
    }

private:
    /**
     * @brief 创建一个新的数据库连接
     * @return 智能指针包装的新连接
     */
    auto CreateConnection() -> std::shared_ptr<sql::Connection>
    {
        // 获取 MySQL 驱动实例
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        // 创建原始连接对象(不直接暴露, 通过智能指针管理)
        sql::Connection *raw_conn = driver->connect(url_, user_, password_);
        raw_conn->setSchema(db_); // 选择要使用的数据库
        borrowed_++;              // 记录借出的连接数

        // 返回定制析构行为的智能指针: 析构时将连接归还到连接池
        return std::shared_ptr<sql::Connection>(
            raw_conn,
            [this](sql::Connection *conn)
            {
                this->ReturnConnection(std::shared_ptr<sql::Connection>(conn));
            });
    }

    std::queue<std::shared_ptr<sql::Connection>> connections_; // 空闲连接队列
    mutable std::mutex mutex_;                                 // 线程安全锁(mutable 允许 const 函数修改)
    std::string url_;                                          // 数据库连接地址
    std::string user_;                                         // 数据库用户名
    std::string password_;                                     // 数据库密码
    std::string db_;                                           // 数据库名称
    size_t borrowed_ = 0;                                      // 当前借出的连接数量
};

// ====================== ORM核心库 ======================
/**
 * @brief ORM(对象关系映射)命名空间, 封装数据库操作的核心逻辑
 *
 * 提供将 C++ 对象与数据库表进行映射的功能, 简化数据库操作,
 * 避免直接编写 SQL 语句, 提高代码可读性和可维护性
 */
namespace ORM
{
    // 字段类型枚举, 定义支持的数据库字段类型
    enum class FieldType
    {
        PrimaryKey, // 主键(通常为自增整数)
        Int,        // 整数类型
        String,     // 字符串类型
        DateTime,   // 日期时间类型
        Double,     // 双精度浮点型
        Boolean,    // 布尔类型
        Blob        // 二进制大对象(用于存储二进制数据)
    };

    /**
     * @brief 字段元数据结构, 存储数据库表字段的描述信息
     */
    struct FieldMeta
    {
        std::string name;     // 字段名称(与数据库表字段名一致)
        FieldType type;       // 字段类型(对应 FieldType 枚举)
        int length;           // 字段长度(主要用于字符串类型)
        bool primary = false; // 是否为主键
    };

    /**
     * @brief 模型基类, 所有业务模型类需继承此类
     * @tparam T 具体业务模型类(CRTP 模式: 奇异递归模板模式)
     */
    template <typename T>
    class Model
    {
    public:
        /**
         * @brief 获取模型对应的数据库表名
         * @return 表名字符串引用
         */
        static auto TableName() -> const std::string &
        {
            return ModelTraits<T>::table_name;
        }

        /**
         * @brief 获取模型对应的字段元数据列表
         * @return 字段元数据向量的引用
         */
        static auto Fields() -> const std::vector<FieldMeta> &
        {
            return ModelTraits<T>::fields;
        }

        /**
         * @brief 获取模型字段的指针列表(用于反射)
         * @return 包含字段指针的元组
         */
        static constexpr auto FieldPointers()
        {
            return ModelTraits<T>::field_pointers();
        }
    };

    /**
     * @brief 模型元特征模板, 每个业务模型需特化此模板
     * @tparam T 具体业务模型类
     *
     * 用于存储模型的元数据(表名、字段信息等), 通过特化实现
     * 不同模型的个性化配置
     */
    template <typename T>
    struct ModelTraits
    {
        static std::string table_name;        // 表名
        static std::vector<FieldMeta> fields; // 字段元数据列表

        /**
         * @brief 字段指针函数, 返回模型字段的指针元组
         * @return 包含字段指针的元组
         */
        static constexpr auto field_pointers()
        {
            return std::make_tuple(); // 默认返回空元组, 需在特化中实现
        }
    };

    // ====================== 预处理语句缓存 ======================
    /**
     * @brief 预处理语句缓存类, 缓存已编译的 SQL 语句
     *
     * 减少重复编译相同 SQL 语句的开销, 提高查询执行效率
     * 线程安全设计, 支持多线程并发访问
     */
    class StatementCache
    {
    public:
        /**
         * @brief 获取指定 SQL 语句的预处理语句
         * @param sql 要执行的 SQL 语句
         * @param conn 数据库连接对象
         * @return 智能指针包装的预处理语句
         *
         * 如果缓存中存在则直接返回, 否则创建新的预处理语句并缓存
         */
        auto Get(
            const std::string &sql,
            sql::Connection *conn) -> std::shared_ptr<sql::PreparedStatement>
        {
            std::lock_guard lock(mutex_); // 加锁保证线程安全

            // 检查缓存中是否存在该 SQL 的预处理语句
            auto it = cache_.find(sql);
            if (it != cache_.end())
            {
                return it->second;
            }

            try
            {
                // 创建新的预处理语句并缓存
                auto stmt = std::shared_ptr<sql::PreparedStatement>(
                    conn->prepareStatement(sql),
                    [](sql::PreparedStatement *pstmt) // 定制析构函数
                    { delete pstmt; });
                cache_[sql] = stmt;
                return stmt;
            }
            catch (sql::SQLException &e)
            {
                // 记录错误日志并重新抛出异常
                Log::lg("error", Log::FileName(), Log::Line(), "Failed to prepare statement: {}", e.what());
                throw;
            }
        }

        /**
         * @brief 清空缓存中的所有预处理语句
         */
        auto Clear() -> void
        {
            std::lock_guard lock(mutex_); // 加锁保证线程安全
            cache_.clear();
        }

    private:
        // 缓存容器: key为SQL语句, value为对应的预处理语句
        std::unordered_map<std::string, std::shared_ptr<sql::PreparedStatement>> cache_;
        std::mutex mutex_; // 线程安全锁
    };

    // ====================== 查询构建器 ======================
    /**
     * @brief 查询构建器类, 用于构建和执行数据库查询
     * @tparam T 要查询的模型类型
     *
     * 提供链式调用接口, 简化SQL查询的构建过程, 支持条件查询、
     * 排序、分页等常见查询操作, 并自动将查询结果映射为模型对象
     */
    template <typename T>
    class QueryBuilder
    {
    public:
        /**
         * @brief 构造函数, 初始化查询构建器
         * @param pool 数据库连接池指针
         */
        explicit QueryBuilder(std::shared_ptr<ConnectionPool> pool)
            : pool_(std::move(pool)) {}

        /**
         * @brief 添加 WHERE 条件
         * @tparam Args 参数包类型
         * @param condition WHERE 子句条件(使用?作为参数占位符)
         * @param args 条件中的参数列表
         * @return 查询构建器自身引用(支持链式调用)
         */
        template <typename... Args>
        auto Where(const std::string &condition, Args &&...args) -> QueryBuilder &
        {
            where_clause = " WHERE " + condition;
            AppendParams(args...); // 收集参数
            return *this;
        }

        /**
         * @brief 添加 ORDER BY 排序
         * @param order 排序条件(如"id DESC")
         * @return 查询构建器自身引用(支持链式调用)
         */
        auto OrderBy(const std::string &order) -> QueryBuilder &
        {
            order_by = " ORDER BY " + order;
            return *this;
        }

        /**
         * @brief 添加 LIMIT 限制返回记录数
         * @param limit 最大返回记录数
         * @return 查询构建器自身引用(支持链式调用)
         */
        auto Limit(int limit) -> QueryBuilder &
        {
            limit_clause = " LIMIT " + std::to_string(limit);
            return *this;
        }

        /**
         * @brief 添加 OFFSET 设置查询偏移量(用于分页)
         * @param offset 偏移量(从第几条记录开始)
         * @return 查询构建器自身引用(支持链式调用)
         */
        auto Offset(int offset) -> QueryBuilder &
        {
            offset_clause = " OFFSET " + std::to_string(offset);
            return *this;
        }

        /**
         * @brief 添加 GROUP BY 分组
         * @param group 分组字段
         * @return 查询构建器自身引用(支持链式调用)
         */
        auto GroupBy(const std::string &group) -> QueryBuilder &
        {
            group_by = " GROUP BY " + group;
            return *this;
        }

        /**
         * @brief 添加 HAVING 条件(与 GROUP BY 配合使用)
         * @tparam Args 参数包类型
         * @param having HAVING 子句条件
         * @param args 条件中的参数列表
         * @return 查询构建器自身引用(支持链式调用)
         */
        template <typename... Args>
        auto Having(const std::string &having, Args &&...args) -> QueryBuilder &
        {
            having_clause = " HAVING " + having;
            AppendParams(args...); // 收集参数
            return *this;
        }

        /**
         * @brief 指定要查询的字段
         * @param columns 字段列表(用逗号分隔, 如"id,name")
         * @return 查询构建器自身引用(支持链式调用)
         *
         * 如果不调用此方法, 默认查询所有字段
         */
        auto Select(const std::string &columns) -> QueryBuilder &
        {
            columns_ = columns;
            return *this;
        }

        /**
         * @brief 执行查询并返回结果
         * @return 模型对象的向量, 每个对象对应一条查询结果
         */
        auto Execute() -> std::vector<T>
        {
            auto conn = pool_->GetConnection(); // 获取数据库连接
            std::string sql = BuildSQL();       // 构建完整SQL语句

            // 记录查询开始时间(用于性能统计)
            auto start = std::chrono::high_resolution_clock::now();

            try
            {
                static StatementCache stmt_cache;             // 预处理语句缓存(单例)
                auto pstmt = stmt_cache.Get(sql, conn.get()); // 获取预处理语句

                BindParameters(pstmt.get()); // 绑定参数到预处理语句

                // 执行查询并获取结果集
                std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

                std::vector<T> results;
                // 遍历结果集并转换为模型对象
                while (res->next())
                {
                    T obj;
                    Deserialize(obj, res.get()); // 反序列化结果到对象
                    results.emplace_back(std::move(obj));
                }

                // 计算查询耗时并记录日志
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                Log::lg("info", Log::FileName(), Log::Line(), "SQL: {} | Duration: {}ms", sql, duration.count());

                return results;
            }
            catch (sql::SQLException &e)
            {
                // 记录错误日志并重新抛出异常
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                Log::lg("error", Log::FileName(), Log::Line(), "SQL Error in query: {} | Duration: {}ms", sql, duration.count());
                Log::lg("error", Log::FileName(), Log::Line(), "Error details: {}", e.what());
                throw;
            }
        }

        /**
         * @brief 异步执行查询
         * @return 包含查询结果的 future 对象
         *
         * 内部使用 std::async 实现异步操作, 调用者可通过 future 获取结果
         */
        auto ExecuteAsync() -> std::future<std::vector<T>>
        {
            return std::async(std::launch::async, [this]
                              { return this->Execute(); });
        }

    private:
        /**
         * @brief 构建完整的 SQL 查询语句
         * @return 构建好的 SQL 字符串
         */
        auto BuildSQL() const -> std::string
        {
            std::string sql = "SELECT ";

            // 处理查询字段: 默认查询所有字段, 否则使用指定字段
            if (columns_.empty())
            {
                for (size_t i = 0; i < Model<T>::Fields().size(); ++i)
                {
                    sql += Model<T>::Fields()[i].name;
                    if (i < Model<T>::Fields().size() - 1)
                        sql += ", ";
                }
            }
            else
            {
                sql += columns_;
            }

            // 拼接 SQL 子句: FROM + 表名 + 其他条件子句
            sql += " FROM " + Model<T>::TableName() +
                   where_clause +
                   group_by +
                   having_clause +
                   order_by +
                   limit_clause +
                   offset_clause;

            return sql;
        }

        /**
         * @brief 将参数绑定到预处理语句
         * @param pstmt 预处理语句对象
         *
         * 根据参数类型自动选择合适的绑定方法, 支持多种常见数据类型
         */
        auto BindParameters(sql::PreparedStatement *pstmt) const -> void
        {
            int idx = 1; // 参数索引(从1开始)
            for (const auto &param : params)
            {
                // 使用std::visit处理variant类型的参数
                std::visit([&](auto &&arg)
                           {
                    using ParamType = std::decay_t<decltype(arg)>;
                    
                    // 根据参数类型选择对应的绑定方法
                    if constexpr (std::is_same_v<ParamType, int>) {
                        pstmt->setInt(idx, arg);
                    } 
                    else if constexpr (std::is_same_v<ParamType, double>) {
                        pstmt->setDouble(idx, arg);
                    } 
                    else if constexpr (std::is_same_v<ParamType, float>) {
                        pstmt->setDouble(idx, static_cast<double>(arg));
                    } 
                    else if constexpr (std::is_same_v<ParamType, std::string>) {
                        pstmt->setString(idx, arg);
                    } 
                    else if constexpr (std::is_same_v<ParamType, const char*>) {
                        pstmt->setString(idx, std::string(arg));
                    } 
                    else if constexpr (std::is_same_v<ParamType, bool>) {
                        pstmt->setBoolean(idx, arg);
                    } 
                    else if constexpr (std::is_same_v<ParamType, std::tm>) {
                        // 日期时间类型转换为字符串
                        std::ostringstream oss;
                        oss << std::put_time(&arg, "%Y-%m-%d %H:%M:%S");
                        pstmt->setString(idx, oss.str());
                    } else if constexpr (std::is_same_v<ParamType, std::vector<uint8_t>>) {
                        // BLOB类型处理: 创建内存流
                        auto data = std::make_shared<std::string>(
                            reinterpret_cast<const char*>(arg.data()), arg.size());
                        auto stream = std::make_shared<std::istringstream>(*data);
                        pstmt->setBlob(idx, stream.get());
                        // 保存流和数据的智能指针, 确保在语句执行期间有效
                        temp_streams_.push_back(stream);
                        temp_data_.push_back(data);
                    }
                    idx++; }, param);
            }
        }

        /**
         * @brief 递归收集查询参数(参数包展开)
         * @tparam Arg 当前参数类型
         * @tparam Args 剩余参数类型
         * @param arg 当前参数
         * @param args 剩余参数
         */
        template <typename Arg, typename... Args>
        auto AppendParams(Arg &&arg, Args &&...args) -> void
        {
            params.push_back(std::forward<Arg>(arg));  // 完美转发参数
            AppendParams(std::forward<Args>(args)...); // 递归处理剩余参数
        }

        /**
         * @brief 参数收集递归终止函数(无参数时调用)
         */
        auto AppendParams() -> void {}

        /**
         * @brief 将结果集反序列化为模型对象
         * @param obj 要赋值的模型对象
         * @param res 数据库查询结果集
         */
        auto Deserialize(T &obj, sql::ResultSet *res) -> void
        {
            const auto &fields = Model<T>::Fields();          // 获取字段元数据
            const auto &pointers = Model<T>::FieldPointers(); // 获取字段指针

            int index = 0;
            // 使用std::apply展开字段指针元组, 逐个处理字段
            std::apply([&](auto &&...field_ptrs)
                       { (DeserializeField(obj, res, fields[index], field_ptrs, index++), ...); }, pointers);
        }

        /**
         * @brief 反序列化单个字段(根据字段名映射)
         * @tparam FieldPtr 字段指针类型
         * @param obj 模型对象
         * @param res 结果集
         * @param field_meta 字段元数据
         * @param field_ptr 字段指针
         * @param index 字段索引
         */
        template <typename FieldPtr>
        auto DeserializeField(T &obj, sql::ResultSet *res,
                              const FieldMeta &field_meta,
                              FieldPtr field_ptr, int index) -> void
        {
            try
            {
                // 根据字段名进行映射(可根据实际模型扩展)
                if (field_meta.name == "id")
                {
                    obj.id = res->getInt(field_meta.name);
                }
                else if (field_meta.name == "name")
                {
                    obj.name = res->getString(field_meta.name);
                }
                else if (field_meta.name == "age")
                {
                    obj.age = res->getInt(field_meta.name);
                }
                else if (field_meta.name == "created_at")
                {
                    // 日期时间字符串转换为std::tm
                    std::string time_str = res->getString(field_meta.name);
                    std::tm tm = {};
                    std::istringstream ss(time_str);
                    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
                    obj.created_at = tm;
                }
                else if (field_meta.name == "is_active")
                {
                    obj.is_active = res->getBoolean(field_meta.name);
                }
                else if (field_meta.name == "avatar")
                {
                    // BLOB类型处理: 读取二进制数据
                    std::istream *blob_stream = res->getBlob(field_meta.name);
                    if (blob_stream)
                    {
                        std::vector<uint8_t> data;
                        char buffer[4096];
                        // 读取流中的所有数据
                        while (blob_stream->read(buffer, sizeof(buffer)) || blob_stream->gcount() > 0)
                        {
                            data.insert(data.end(), buffer, buffer + blob_stream->gcount());
                        }
                        obj.avatar = std::move(data);
                        delete blob_stream; // 释放流资源
                    }
                }
            }
            catch (sql::SQLException &e)
            {
                // 记录字段反序列化错误
                Log::lg("error", Log::FileName(), Log::Line(), "Error deserializing field {}: {}", field_meta.name, e.what());
                throw;
            }
        }

        std::shared_ptr<ConnectionPool> pool_; // 数据库连接池
        // 查询参数列表(使用 variant 支持多种类型)
        std::vector<std::variant<int, double, float, std::string, bool, const char *, std::tm, std::vector<uint8_t>>> params;
        std::string where_clause;  // WHERE 子句
        std::string order_by;      // ORDER BY 子句
        std::string limit_clause;  // LIMIT 子句
        std::string offset_clause; // OFFSET 子句
        std::string group_by;      // GROUP BY 子句
        std::string having_clause; // HAVING 子句
        std::string columns_;      // 要查询的字段

        // 临时存储 BLOB 类型的流和数据(确保生命周期)
        mutable std::vector<std::shared_ptr<std::istringstream>> temp_streams_;
        mutable std::vector<std::shared_ptr<std::string>> temp_data_;
    };

} // namespace ORM

// ====================== 全局辅助函数和宏 ======================
/**
 * @brief 获取全局数据库连接池实例
 * @return 连接池的智能指针
 *
 * 采用单例模式, 确保全局只有一个连接池实例
 * 默认连接本地 MySQL 数据库, 用户需根据实际情况修改连接参数
 */
inline auto GetGlobalPool() -> std::shared_ptr<ConnectionPool>
{
    static auto pool = std::make_shared<ConnectionPool>(
        "tcp://127.0.0.1:3306", // 数据库地址
        "username",             // 用户名(需修改)
        "password",             // 密码(需修改)
        "database");            // 数据库名(需修改)
    return pool;
}

/**
 * @brief 创建指定模型的查询构建器
 * @tparam T 模型类型
 * @return 查询构建器对象
 */
namespace ORM
{
    template <typename T>
    auto Query() -> QueryBuilder<T>
    {
        return QueryBuilder<T>(::GetGlobalPool());
    }
}

// 模型定义辅助宏(简化模型元数据的特化)

/**
 * @brief 声明模型字段元数据
 * @param FieldName 字段名
 * @param Type 字段类型(对应 FieldType 枚举)
 * @param ... 可变参数(主要用于指定长度)
 *
 * 示例: ORM_FIELD_DECL(id, PrimaryKey, 0, true)
 */
#define ORM_FIELD_DECL(FieldName, Type, ...) \
    {#FieldName, ORM::FieldType::Type, ##__VA_ARGS__}

/**
 * @brief 获取模型字段的指针
 * @param ModelType 模型类型
 * @param FieldName 字段名
 * @return 字段成员指针
 */
#define ORM_FIELD_MEMBER(ModelType, FieldName) &ModelType::FieldName

/**
 * @brief 声明模型的元数据(表名和字段列表)
 * @param model_name 模型类名
 * @param ... 字段元数据列表(使用 ORM_FIELD_DECL 宏)
 */
#define ORM_MODEL_DECL(model_name, ...)                                 \
    template <>                                                         \
    std::string ORM::ModelTraits<model_name>::table_name = #model_name; \
    template <>                                                         \
    std::vector<ORM::FieldMeta> ORM::ModelTraits<model_name>::fields = {__VA_ARGS__};

/**
 * @brief 声明模型的字段指针(用于反射)
 * @param model_name 模型类名
 * @param ... 字段指针列表(使用 ORM_FIELD_MEMBER 宏)
 */
#define ORM_MODEL_MEMBERS(model_name, ...)                        \
    template <>                                                   \
    constexpr auto ORM::ModelTraits<model_name>::field_pointers() \
    {                                                             \
        return std::make_tuple(__VA_ARGS__);                      \
    }
