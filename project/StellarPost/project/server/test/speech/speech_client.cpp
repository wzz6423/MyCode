// 测试通过

// speech_client 测试客户端实现

// C++
#include <iostream>
#include <print>
// speech server
#include "../../../project/speech/source/speech_server.hpp"
#include "speech.pb.h"
// gflags
#include <gflags/gflags.h>

DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(speechService, "/service/speech_service/", "当前实例名称");

auto main(int argc, char *argv[]) -> int
{
    // 1.进行服务发现(参数解析) -- 发现 speech server 服务器节点地址信息并实例化通信信道
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 构建 rpc 信道管理对象
    stellar_post::Brpc::ChannelManager::managerPtr ptr = std::make_shared<stellar_post::Brpc::ChannelManager>();
    ptr->Declared(FLAGS_speechService);

    auto putCb = std::bind(&stellar_post::Brpc::ChannelManager::Online, ptr.get(), std::placeholders::_1, std::placeholders::_2);
    auto delCb = std::bind(&stellar_post::Brpc::ChannelManager::Offline, ptr.get(), std::placeholders::_1, std::placeholders::_2);

    // 构建服务注册发现对象
    stellar_post::Etcd::Discovery::discoveryPtr discovery = std::make_shared<stellar_post::Etcd::Discovery>(FLAGS_registryHost,
                                                                                                            FLAGS_baseService,
                                                                                                            putCb, delCb);

    // 通过 rpc 信道管理对象获取提供 speech server 服务的信道
    auto channel = ptr->Get(FLAGS_speechService);
    auto chan = channel.value();

    // 读取语音文件数据
    std::string file_content;
    aip::get_file_content("16k-test.pcm", &file_content);

    // 发起 speech server 语音识别的 rpc 调用
    stellar_post::speech::SpeechService_Stub stub(chan.get());
    stellar_post::specch::SpeechRecognitionReq req;
    req.speech_content(file_content);
    req.set_request_id("1234567890");
    brpc::Controller *cntl = new brpc::Controller();
    stellar_post::specch::SpeechRecognitionRsp *rsp = new stellar_post::specch::SpeechRecognitionRsp();
    // 同步
    stub.SpeechRecognition(cntl, &req, rsp, nullptr);
    if (cntl->Failed()) // 检查 rpc 调用是否失败
    {
        std::cerr << "RPC 调用失败: " << cntl->ErrorText() << std::endl;
    }
    if (rsp->success() == false)
    {
        std::println("{}", rsp->errmag());
        return 1;
    }
    std::println("收到响应: {} : {}", rsp->message(), rsp->recognition_result());
    delete rsp;
    delete cntl;

    return 0;
}