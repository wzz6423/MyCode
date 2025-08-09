// 测试通过

#include "../../../project/speech/source/asr/asr.hpp"
#include <print>
#include <gflags/gflags.h>

DEFINE_string(appId, "your_app_id", "ASR 平台应用 ID");
DEFINE_string(apiKey, "your_api_key", "ASR 平台访问密钥 Key");
DEFINE_string(apiSecret, "your_api_secret", "ASR 平台加密密钥 Secret");

auto main(int argc, char *argv[]) -> int
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    stellar_post::ASR::ASRClient asr(FLAGS_appId, FLAGS_apiKey, FLAGS_apiSecret);

    std::string file_content;
    aip::get_file_content("16k-test.pcm", &file_content); // 北京科技馆
    std::optional<std::string> ret = asr.Recognize(file_content);
    if (ret)
    {
        std::println("{}", *ret);
    }
    else
    {
        std::println("error");
    }

    return 0;
}