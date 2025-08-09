#include "./aip-cpp-sdk/speech.h"

void asr(aip::Speech &client)
{
    // 无可选参数调用接口
    std::string file_content;
    aip::get_file_content("16k-test.pcm", &file_content); // 北京科技馆

    // 如果需要覆盖或者加入参数
    // std::map<std::string, std::string> options;
    // options["dev_pid"] = "1537"; // 默认普通话
    // 1737	英语
    // 1637	粤语
    // Json::Value result = client.recognize(file_content, "pcm", 16000, options);
    Json::Value result = client.recognize(file_content, "pcm", 16000, aip::null);
    if (result["err_no"].asInt() == 0)
    {
        std::cout << "识别结果: " << result["result"][0].asString() << std::endl;
    }
    else
    {
        std::cout << "识别失败: " << result["error_msg"].asString() << std::endl;
    }
}

auto main() -> int
{
    // 设置APPID/AK/SK
    std::string app_id = "你的 App ID";
    std::string api_key = "你的 Api key";
    std::string secret_key = "你的 Secret Key";
    aip::Speech client(app_id, api_key, secret_key);

    asr(client);

    return 0;
}