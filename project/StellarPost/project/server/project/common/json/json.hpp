#pragma once

/*
    -ljsoncpp
*/

// C++
#include <string>
#include <memory>
// json
#include <json/json.h>
// Other
#include "../log/logger.hpp"

namespace stellar_post
{
    namespace json
    {
        bool Serialize(const Json::Value &val, std::string *dst)
        {
            // 先定义 Json::StreamWriter 工厂类 Json::StreamWriterBuilder
            Json::StreamWriterBuilder swb;
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
                Log::lg("error", Log::FileName(), Log::Line(), "Json 序列化失败!");
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
                Log::lg("error", Log::FileName(), Log::Line(), "Json 反序列化出现错误: {}", err);
                return false;
            }
            else
            {
                return true;
            }
        }
    }
}