#pragma once

/*
    /usr/lib/x86_64-linux-gnu/libjsoncpp.so.1.8.4 -lcurl -lcrypto
*/

// C++
#include <string>
#include <memory>
#include <optional>
// baidu asr sdk
#include "./aip-cpp-sdk/speech.h"
// Other
#include "../../common/log/logger.hpp"

namespace stellar_post
{
    namespace ASR
    {
        class ASRClient
        {
        public:
            using asrPtr = std::shared_ptr<ASRClient>;

        public:
            ASRClient(const std::string &appId, const std::string &apiKey, const std::string &secretKey)
            {
                _client = std::make_unique<aip::Speech>(appId, apiKey, secretKey);
            }

            ~ASRClient() = default;

            std::optional<std::string> Recognize(const std::string &speechData)
            {
                // 如果需要覆盖或者加入参数
                // std::map<std::string, std::string> options;
                // options["dev_pid"] = "1537"; // 默认普通话
                // 1737	英语
                // 1637	粤语
                // Json::Value result = client.recognize(speechData, "pcm", 16000, options);
                Json::Value result = _client->recognize(speechData, "pcm", 16000, aip::null);
                if (result["err_no"].asInt() != 0)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "识别失败: {}", result["error_msg"].asString());
                    return std::nullopt;
                }
                else
                {
                    return result["result"][0].asString();
                }
            }

        private:
            std::unique_ptr<aip::Speech> _client;
        };
    }
}