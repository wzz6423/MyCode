#include "MySQL_operator.hpp"
#include <print>  // 包含std::println所需的头文件

// 定义User类，继承自ORM模型基类
class User : public ORM::Model<User>
{
public:
    int id;
    std::string name;
    int age;
    std::tm created_at;
    bool is_active;
    std::vector<uint8_t> avatar;

    // 转换为字符串表示
    std::string ToString() const
    {
        std::stringstream ss;
        ss << "User(id=" << id << ", name='" << name
           << "', age=" << age << ", is_active=" << std::boolalpha << is_active
           << ", avatar_size=" << avatar.size() << ")";
        return ss.str();
    }
};

// 声明模型元数据：表名和字段信息
ORM_MODEL_DECL(User,
    ORM_FIELD_DECL(id, PrimaryKey, 0, true),  // 主键字段
    ORM_FIELD_DECL(name, String, 50),         // 字符串字段，长度50
    ORM_FIELD_DECL(age, Int),                 // 整数字段
    ORM_FIELD_DECL(created_at, DateTime),     // 日期时间字段
    ORM_FIELD_DECL(is_active, Boolean),       // 布尔字段
    ORM_FIELD_DECL(avatar, Blob)              // 二进制数据字段
);

// 定义成员指针，用于ORM反射
ORM_MODEL_MEMBERS(User,
    ORM_FIELD_MEMBER(User, id),
    ORM_FIELD_MEMBER(User, name),
    ORM_FIELD_MEMBER(User, age),
    ORM_FIELD_MEMBER(User, created_at),
    ORM_FIELD_MEMBER(User, is_active),
    ORM_FIELD_MEMBER(User, avatar)
);

int main()
{
    try
    {
        // 同步查询示例
        {
            std::println("开始同步查询...");

            // 链式构建查询
            auto users = ORM::Query<User>()
                             .Where("age > ? AND is_active = ?", 18, true)
                             .OrderBy("age DESC")
                             .GroupBy("name")
                             .Having("COUNT(*) > ?", 1)
                             .Limit(10)
                             .Offset(5)
                             .Execute();

            std::println("同步查询返回 {} 条记录", users.size());
            for (const auto &user : users)
            {
                std::println("用户: {}", user.ToString());
            }
        }

        // 异步查询示例
        {
            std::println("\n开始异步查询...");

            auto future = ORM::Query<User>()
                              .Where("age > ?", 25)
                              .OrderBy("name ASC")
                              .Limit(5)
                              .ExecuteAsync();

            std::println("等待异步查询结果...");
            auto users = future.get();  // 阻塞等待异步结果

            std::println("异步查询返回 {} 条记录", users.size());
            for (const auto &user : users)
            {
                std::println("异步用户: {}", user.ToString());
            }
        }
    }
    catch (const std::exception &e)
    {
        std::println(std::cerr, "程序异常: {}", e.what());
        return 1;
    }

    return 0;
}
