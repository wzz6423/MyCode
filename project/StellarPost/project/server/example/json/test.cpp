// C++
#include <print>
#include <string>
#include <sstream>
#include <memory>
// json
#include <json/json.h>

bool Serialize(const Json::Value &val, std::string *dst)
{
    // 先定义 Json::StreamWriter 工厂类 Json::StreamWriterBuilder
    Json::StreamWriterBuilder swb;
    swb.settings_["emitUTF8"] = true;
    std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

    // 通过 Json::StreamWriter 中的 write 接口进行序列化
    std::stringstream ss;
    int ret = sw->write(val, &ss);
    if (ret == 0)
    {
        *dst = ss.str();
        return true;
    }
    else
    {
        std::println("Json 序列化失败!");
        return false;
    }
}

bool UnSerialize(const std::string &src, Json::Value *val)
{
    // 先定义 Json::CharReader 工厂类 Json::CharReaderBuilder
    Json::CharReaderBuilder crb;
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());

    std::string err;
    // 通过 Json::CharReader 中的 parse 接口进行反序列化
    cr->parse(src.c_str(), src.c_str() + src.size(), val, &err);
    if (err.empty() == false)
    {
        std::println("Json 反序列化出现错误: {}", err);
        return false;
    }
    else
    {
        return true;
    }
}

auto main() -> int
{
    const char *name = "张三";
    int age = 20;
    float score[] = {88, 89.5, 90.8};

    Json::Value stu;
    stu["姓名"] = name;
    stu["年龄"] = age;
    stu["成绩"].append(score[0]);
    stu["成绩"].append(score[1]);
    stu["成绩"].append(score[2]);

    std::string jsonStu;
    bool ret;

    ret = Serialize(stu, &jsonStu);
    if (ret == false)
    {
        return 1;
    }
    std::println("序列化后的字符串: {}", jsonStu);

    Json::Value val;
    ret = UnSerialize(jsonStu, &val);
    if (ret == false)
    {
        return 2;
    }
    std::println("反序列化后的字符串: 姓名: {}, 年龄: {}, 分数: {}-{}-{}", 
        val["姓名"].asString(), val["年龄"].asInt(), val["成绩"][0].asFloat(), 
        val["成绩"][1].asFloat(), val["成绩"][2].asFloat());

    return 0;
}