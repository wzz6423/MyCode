// 测试通过

// file_client 测试客户端实现
// 封装 4 个接口进行 rpc 调用, 实现对于 4 个业务接口的测试

// C++
#include <iostream>
#include <print>
#include <functional>
// file server
#include "../source/file_server.hpp"
#include "../build/file.pb.h"
// gflags
#include <gflags/gflags.h>
// gtest
#include <gtest/gtest.h>

DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(fileService, "/service/file_service", "当前实例名称");

stellar_post::Brpc::Channel::brpcChannelPtr chan;
std::string signal_file_ID = "";
std::string mul_file_ID_1 = "";
std::string mul_file_ID_2 = "";

TEST(put_test, signal_file)
{
    // 1.读取当前目录下的指定文件数据
    std::string body;
    ASSERT_TRUE(stellar_post::Utils::ReadFile("./Makefile", &body));

    // 2.实例化 rpc 调用客户端对象, 发起 rpc 调用
    // 发起 file server 语音识别的 rpc 调用
    stellar_post::file::FileService_Stub stub(chan.get());
    stellar_post::file::PutSingleFileReq req;
    req.set_request_id("1111");
    req.mutable_file_data()->set_file_name("Makefile");
    req.mutable_file_data()->set_file_content(body);
    req.mutable_file_data()->set_file_size(body.size());

    brpc::Controller *cntl = new brpc::Controller();
    stellar_post::file::PutSingleFileRsp *rsp = new stellar_post::file::PutSingleFileRsp();
    stub.PutSingleFile(cntl, &req, rsp, nullptr);

    // 3.检测返回值中是否上传成功
    ASSERT_TRUE(cntl->Failed() == false);
    ASSERT_TRUE(rsp->success());
    ASSERT_EQ(rsp->file_info().file_size(), body.size());
    ASSERT_EQ(rsp->file_info().file_name(), "Makefile");
    signal_file_ID = rsp->file_info().file_id();
    std::println("file ID : {}", rsp->file_info().file_id());
    delete rsp;
    delete cntl;
}

TEST(get_test, signal_file)
{
    // 发起 rpc 调用进行文件下载
    stellar_post::file::FileService_Stub stub(chan.get());
    stellar_post::file::GetSingleFileReq req;
    brpc::Controller *cntl = new brpc::Controller();
    stellar_post::file::GetSingleFileRsp *rsp = new stellar_post::file::GetSingleFileRsp();
    req.set_request_id("2222");
    req.set_file_id(signal_file_ID);

    stub.GetSingleFile(cntl, &req, rsp, nullptr);
    // 检测返回值中是否上传成功
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp->success());

    // 将文件数据写入文件中
    ASSERT_EQ(signal_file_ID, rsp->file_data().file_id());
    stellar_post::Utils::WriteFile("make_file_download.txt", rsp->file_data().file_content());
    delete rsp;
    delete cntl;
}

TEST(put_test, mul_file)
{
    // 1.读取当前目录下的指定文件数据
    std::string body;
    ASSERT_TRUE(stellar_post::Utils::ReadFile("./file.pb.h", &body));
    std::string body2;
    ASSERT_TRUE(stellar_post::Utils::ReadFile("./base.pb.h", &body2));

    // 2.实例化 rpc 调用客户端对象, 发起 rpc 调用
    // 发起 file server 语音识别的 rpc 调用
    stellar_post::file::FileService_Stub stub(chan.get());
    stellar_post::file::PutMultiFileReq req;
    req.set_request_id("3333");

    auto file_data = req.add_file_data();
    file_data->set_file_name("file.pb.h");
    file_data->set_file_content(body);
    file_data->set_file_size(body.size());

    file_data = req.add_file_data();
    file_data->set_file_name("base.pb.h");
    file_data->set_file_content(body2);
    file_data->set_file_size(body2.size());

    brpc::Controller *cntl = new brpc::Controller();
    stellar_post::file::PutMultiFileRsp *rsp = new stellar_post::file::PutMultiFileRsp();
    stub.PutMultiFile(cntl, &req, rsp, nullptr);

    // 3.检测返回值中是否上传成功
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp->success());
    for (size_t i = 0; i < 2; ++i)
    {
        ASSERT_EQ(rsp->file_info(i).file_size(), i == 0 ? body.size() : body2.size());
        ASSERT_EQ(rsp->file_info(i).file_name(), i == 0 ? "file.pb.h" : "base.pb.h");
    }
    mul_file_ID_1 = rsp->file_info(0).file_id();
    mul_file_ID_2 = rsp->file_info(1).file_id();
    delete rsp;
    delete cntl;
}

TEST(get_test, mul_file)
{
    // 发起 rpc 调用进行文件下载
    stellar_post::file::FileService_Stub stub(chan.get());
    stellar_post::file::GetMultiFileReq req;
    brpc::Controller *cntl = new brpc::Controller();
    stellar_post::file::GetMultiFileRsp *rsp = new stellar_post::file::GetMultiFileRsp();
    req.set_request_id("4444");
    req.add_file_id_list(mul_file_ID_1);
    req.add_file_id_list(mul_file_ID_2);

    stub.GetMultiFile(cntl, &req, rsp, nullptr);
    // 检测返回值中是否上传成功
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp->success());

    // 将文件数据写入文件中
    ASSERT_TRUE(rsp->file_data().find(mul_file_ID_1) != rsp->file_data().end());
    ASSERT_TRUE(rsp->file_data().find(mul_file_ID_2) != rsp->file_data().end());
    stellar_post::Utils::WriteFile("file_file1_download.txt", rsp->file_data().at(mul_file_ID_1).file_content());
    stellar_post::Utils::WriteFile("base_file2_download.txt", rsp->file_data().at(mul_file_ID_2).file_content());
    delete rsp;
    delete cntl;
}

auto main(int argc, char *argv[]) -> int
{
    testing::InitGoogleTest(&argc, argv);
    // 1.进行服务发现(参数解析) -- 发现 file server 服务器节点地址信息并实例化通信信道
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 构建 rpc 信道管理对象
    stellar_post::Brpc::ChannelManager::managerPtr ptr = std::make_shared<stellar_post::Brpc::ChannelManager>();
    ptr->Declared(FLAGS_fileService);

    auto putCb = std::bind(&stellar_post::Brpc::ChannelManager::Online, ptr.get(), std::placeholders::_1, std::placeholders::_2);
    auto delCb = std::bind(&stellar_post::Brpc::ChannelManager::Offline, ptr.get(), std::placeholders::_1, std::placeholders::_2);

    // 构建服务注册发现对象
    stellar_post::Etcd::Discovery::discoveryPtr discovery = std::make_shared<stellar_post::Etcd::Discovery>(FLAGS_registryHost,
                                                                                                            FLAGS_baseService,
                                                                                                            putCb, delCb);

    // 通过 rpc 信道管理对象获取提供 file server 服务的信道
    auto channel = ptr->Get(FLAGS_fileService);
    if (channel)
    {
        chan = channel.value();
    }
    else
    {
        std::println("file service 宕机，测试文件上传、下载失败");
        exit(-1);
    }

    return RUN_ALL_TESTS(); // 执行所有测试用例
}