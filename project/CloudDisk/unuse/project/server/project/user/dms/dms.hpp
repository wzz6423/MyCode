#pragma once

/*
    -lalibabacloud-sdk-core
*/

// C++
#include <iostream>
#include <string>
#include <memory>
// C
#include <cstdlib>
// alibaba dms sdk
#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/core/CommonRequest.h>
#include <alibabacloud/core/CommonClient.h>
#include <alibabacloud/core/CommonResponse.h>
// Other
#include "../../common/log/logger.hpp" // 日志器 spdlog 模块封装

namespace cloud_disk::DMS
{
    class DMSClient
    {
    public:
        using dmsPtr = std::shared_ptr<DMSClient>;

    public:
        DMSClient(const std::string &accessKeyId, const std::string &accessKeySecret)
        {
            AlibabaCloud::InitializeSdk();
            AlibabaCloud::ClientConfiguration configuration("cn-qingdao");

            // 超时信息设置
            configuration.setConnectTimeout(1500);
            configuration.setReadTimeout(4000);

            AlibabaCloud::Credentials credential(accessKeyId, accessKeySecret);
            _client = std::make_unique<AlibabaCloud::CommonClient>(credential, configuration);
        }

        auto Send(const std::string &phoneNumber, const std::string &code) -> bool
        {
            AlibabaCloud::CommonRequest request(AlibabaCloud::CommonRequest::RequestPattern::RpcPattern);
            request.setHttpMethod(AlibabaCloud::HttpRequest::Method::Post);

            request.setDomain("dysmsapi.aliyuncs.com");
            request.setVersion("2017-05-25");
            request.setQueryParameter("Action", "SendSms");
            request.setQueryParameter("SignName", "CloudDisk");
            request.setQueryParameter("TemplateCode", "SMS_465324787");

            request.setQueryParameter("PhoneNumbers", phoneNumber);

            std::string paramCode = R"({"code":")";
            paramCode += code;
            paramCode += R"("})";
            request.setQueryParameter("TemplateParam", paramCode);

            auto response = _client->commonResponse(request);
            if (!response.isSuccess())
            {
                Log::lg("error", Log::FileName(), Log::Line(),
                        "error: {}, request id: {}", response.error().errorMessage(), response.error().requestId());
                return false;
            }
            return true;
        }

        ~DMSClient()
        {
            AlibabaCloud::ShutdownSdk();
        }

    private:
        std::unique_ptr<AlibabaCloud::CommonClient> _client;
    };
}