#pragma once

// C++
#include <string>
#include <vector>
#include <sstream>
// Other
#include "./httplib/httplib.h"
#include "Data.hpp"

extern CloudBackUp::Data::DataManager *_data;

namespace CloudBackUp
{
    namespace Server
    {
        class Server
        {
        public:
            Server()
            {
                Config::Config *cfg = Config::Config::GetInstance();
                _server_port = cfg->GetServerPort();
                _server_ip = cfg->GetServerIp();
                _download_prefix = cfg->GetDownloadPrefix();
            }

            void Run()
            {
                _server.Post("/upload", Upload);
                _server.Get("/listshow", ListShow);
                _server.Get("/", ListShow);
                std::string downloadUrl = _download_prefix + "(.*)";
                _server.Get(downloadUrl, Download);
                _server.listen(_server_ip.c_str(), _server_port);
            }

        private:
            // 业务处理函数
            // 上传
            static void Upload(const httplib::Request &req, httplib::Response &rsp)
            {
                // 文件数据存在正文, 正文不全是文件数据
                // 判断是否存在上传文件的区域
                auto ret = req.has_file("file");
                if (ret == false)
                {
                    rsp.status = 400;
                    return;
                }
                const auto &file = req.get_file_value("file");
                std::string _back_dir = Config::Config::GetInstance()->GetBackDir();
                std::string realPath = _back_dir + Util::FileUtil(file.filename).Filename(); // file.filename: 文件名称 -- 文件实际存储路径
                Util::FileUtil fu(realPath);
                std::cout << realPath <<std::endl;
                fu.SetContent(file.content); // file.content: 文件数据

                Data::BackupInfo info(realPath); // 组织备份文件信息
                _data->Insert(info);             // 插入数据到数据管理模块
            }

            // 展示页面
            static void ListShow(const httplib::Request &req, httplib::Response &rsp)
            {
                // html 文件信息
                // std::string html_file_pre = "../project/wwwroot/index_pre.html"; // for test
                // std::string html_file_aft = "../project/wwwroot/index_after.html";
                std::string html_file_pre = "../wwwroot/index_pre.html";
                std::string html_file_aft = "../wwwroot/index_after.html";
                std::string html_pre;
                std::string html_aft;
                Util::FileUtil(html_file_pre).GetContent(&html_pre);
                Util::FileUtil(html_file_aft).GetContent(&html_aft);
                // 组织 html 文件所需的两个字段, 其实可以不需要, 这里只是稍微美观一些
                std::string file_pre = R"(<div class="file-item">)";
                std::string file_mid = R"(<a href=")";
                std::string file_aft = R"(" class="download-btn">下载</a></div>)";
                std::string span_pre = R"(<span>)";
                std::string span_aft = R"(</span>)";

                // 获取所有文件备份信息
                std::vector<Data::BackupInfo> all;
                _data->GetAll(&all);

                // 根据所有备份信息组织 html 文件数据
                std::stringstream html;
                // 前面的 html 组织信息
                html << html_pre;
                for (auto &e : all)
                {
                    // 文件展示模块前面部分信息
                    html << file_pre;

                    // 文件名
                    html << span_pre;
                    html << Util::FileUtil(e._real_path).Filename();
                    html << span_aft;

                    // 其他属性
                    html << span_pre;
                    html << Util::FormatExistTime(e._modify_time); // 时间
                    html << span_aft;

                    html << span_pre;
                    html << e._file_size; // 文件大小
                    html << span_aft;

                    // 下载 url
                    html << file_mid;
                    html << e._url;

                    // 文件展示模块后面部分信息
                    html << file_aft;
                }
                // 后面的 html 组织信息
                html << html_aft;

                rsp.body = html.str();
                rsp.set_header("Content-Type", "text/html");
                rsp.status = 200;
            }

            static std::string ETag(const Data::BackupInfo &info)
            {
                // file name-file size-last modify time
                Util::FileUtil fu(info._real_path);
                std::string etag = fu.Filename();
                etag += "-";
                etag += std::to_string(info._file_size);
                etag += "-";
                etag += std::to_string(info._modify_time);
                return std::move(etag);
            }

            // 下载
            static void Download(const httplib::Request &req, httplib::Response &rsp)
            {
                // 获取客户端请求的资源路径 path -- req 中
                // 根据资源路径, 获取文件备份信息
                Data::BackupInfo info;
                _data->Get1ByURL(req.path, &info);

                // 判断文件是否被压缩, 如果被压缩要先解压缩, 删除压缩包并修改备份信息
                if (info._is_pack == true)
                {
                    Util::FileUtil fu(info._pack_path);
                    fu.UnCompress(info._real_path);
                    fu.Remove();
                    info._is_pack = false;
                    _data->Update(info);
                }

                // 判断是否断点续传
                bool retrans = false;
                std::string old_etag;
                if (req.has_header("If-Range"))
                {
                    old_etag = req.get_header_value("If-Range");
                    // 有 If-Range 字段且值与请求文件最新 ETag 相同则进行断点续传
                    if (old_etag == ETag(info))
                    {
                        retrans = true;
                    }
                }

                // 读取文件数据, 放入 rsp.body
                Util::FileUtil fu(info._real_path);
                fu.GetContent(&rsp.body);

                if (retrans == false)
                {
                    // 设置响应头部字段 -- ETag & Accept-Ranges: bytes
                    rsp.set_header("Content-Type", "application/octet-stream");
                    rsp.set_header("ETag", ETag(info));
                    rsp.set_header("Accept-Ranges", "bytes");
                    rsp.status = 200;
                }
                else
                {
                    // httplib 库已支持断点续传, 所以其实不改也行
                    rsp.set_header("Content-Type", "application/octet-stream");
                    rsp.set_header("ETag", ETag(info));
                    rsp.set_header("Accept-Ranges", "bytes");
                    rsp.status = 206; // 区间请求响应 206
                }
            }

        private:
            int _server_port;
            std::string _server_ip;
            std::string _download_prefix;
            httplib::Server _server;
        };
    }
}