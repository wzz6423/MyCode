// https://next.api.aliyun.com/api/Dysmsapi/2017-05-25/SubmitSmsQualification?RegionId=cn-qingdao&tab=DEMO&sdkStyle=old&lang=CPP
// 采用阿里进行短信发送的示例代码
#include <cstdlib>
#include <iostream>
#include <string>
#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/core/CommonRequest.h>
#include <alibabacloud/core/CommonClient.h>
#include <alibabacloud/core/CommonResponse.h>

using namespace std;
using namespace AlibabaCloud;

int main(int argc, char **argv)
{
    AlibabaCloud::InitializeSdk();
    AlibabaCloud::ClientConfiguration configuration("cn-qingdao");
    // specify timeout when create client.
    configuration.setConnectTimeout(1500);
    configuration.setReadTimeout(4000);
    // Please ensure that the environment variables ALIBABA_CLOUD_ACCESS_KEY_ID and ALIBABA_CLOUD_ACCESS_KEY_SECRET are set.
    // 更改 API 密钥: "ALIBABA_CLOUD_ACCESS_KEY_ID" & "ALIBABA_CLOUD_ACCESS_KEY_SECRET"
    // AlibabaCloud::Credentials credential( getenv("ALIBABA_CLOUD_ACCESS_KEY_ID"), getenv("ALIBABA_CLOUD_ACCESS_KEY_SECRET") );
    // 样例代码通过获取环境变量
    std::string accessKeyId = "ALIBABA_CLOUD_ACCESS_KEY_ID";
    std::string accessKeySecret = "ALIBABA_CLOUD_ACCESS_KEY_SECRET";
    AlibabaCloud::Credentials credential(accessKeyId, accessKeySecret);
    /* use STS Token
    credential.setSessionToken( getenv("ALIBABA_CLOUD_SECURITY_TOKEN") );
    */
    AlibabaCloud::CommonClient client(credential, configuration);
    AlibabaCloud::CommonRequest request(AlibabaCloud::CommonRequest::RequestPattern::RpcPattern);
    request.setHttpMethod(AlibabaCloud::HttpRequest::Method::Post);
    request.setDomain("dysmsapi.aliyuncs.com");
    request.setVersion("2017-05-25");
    request.setQueryParameter("Action", "SendSms");
    request.setQueryParameter("SignName", "StellarPost");
    request.setQueryParameter("TemplateCode", "SMS_465324787");
    request.setQueryParameter("PhoneNumbers", "aaaaaaaaa");
    request.setQueryParameter("TemplateParam","{\"code\":\"1234\"}");
    // request相关代码修改

    auto response = client.commonResponse(request);
    if (response.isSuccess())
    {
        printf("request success.\n");
        printf("result: %s\n", response.result().payload().c_str());
    }
    else
    {
        printf("error: %s\n", response.error().errorMessage().c_str());
        printf("request id: %s\n", response.error().requestId().c_str());
    }

    AlibabaCloud::ShutdownSdk();
    return 0;
}