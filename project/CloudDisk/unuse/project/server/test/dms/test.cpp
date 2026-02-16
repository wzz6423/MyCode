// 测试通过

#include "../../project/user/dms/dms.hpp"
#include <gflags/gflags.h>

DEFINE_string(keyId, "your_access_key_id", "DMS 平台访问密钥 ID");
DEFINE_string(keySecret, "your_access_key_secret", "DMS 平台访问密钥 Secret");
DEFINE_string(phoneNumber, "your_phone_number", "发送验证码的电话号码");
DEFINE_string(Code, "send_code", "发送验证码的内容");

auto main(int argc, char *argv[]) -> int
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    cloud_disk::DMS::DMSClient client(FLAGS_keyId, FLAGS_keySecret);
    client.Send(FLAGS_phoneNumber, FLAGS_Code);

    return 0;
}