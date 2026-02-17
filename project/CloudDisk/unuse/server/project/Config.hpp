#pragma once

// C++
#include <mutex>
// Other
#include "util.hpp"

namespace CloudBackUp
{
    namespace Config
    {
        class Config
        {
        public:
            static Config *GetInstance()
            {
                if (_instance == nullptr)
                {
                    {
                        std::unique_lock<std::mutex> lock(_mtx);
                        if (_instance == nullptr)
                        {
                            _instance = new Config();
                        }
                    }
                }
                return _instance;
            }

            int GetHotTime() const
            {
                return _hot_time;
            }
            int GetServerPort() const
            {
                return _server_port;
            }
            std::string GetServerIp() const
            {
                return _server_ip;
            }
            std::string GetDownloadPrefix() const
            {
                return _download_prefix;
            }
            std::string GetPackfileSuffix() const
            {
                return _packfile_suffix;
            }
            std::string GetPackDir() const
            {
                return _pack_dir;
            }
            std::string GetBackDir() const
            {
                return _back_dir;
            }
            std::string GetBackupFile() const
            {
                return _backup_file;
            }

        private:
            int _hot_time;                // 热点时间
            int _server_port;             // 服务器监听 port
            std::string _server_ip;       // 服务器 ip
            std::string _download_prefix; // 访问下载 url 前缀路径
            std::string _packfile_suffix; // 压缩文件文件后缀
            std::string _pack_dir;        // 压缩文件存放目录
            std::string _back_dir;        // 备份文件存放目录
            std::string _backup_file;     // 数据信息存放文件

        private:
            Config(const Config &) = delete;
            Config &operator=(const Config &) = delete;

            Config()
            {
                if (ReadConfigFile() == false)
                {
                    std::cerr << "read config file error!" << std::endl;
                }
            }
            // 读取配置文件
            bool ReadConfigFile()
            {
                Util::FileUtil fu(_default_config_file);
                std::string body;
                if (fu.GetContent(&body) == false)
                {
                    std::cerr << "read config file error!" << std::endl;
                    return false;
                }

                Json::Value root;
                if (Util::JsonUtil::UnSerialize(body, &root) == false)
                {
                    std::cerr << "parse config file error!" << std::endl;
                    return false;
                }

                _hot_time = root["hot_time"].asInt();
                _server_port = root["server_port"].asInt();
                _server_ip = root["server_ip"].asString();
                _download_prefix = root["download_prefix"].asString();
                _packfile_suffix = root["packfile_suffix"].asString();
                _pack_dir = root["pack_dir"].asString();
                _back_dir = root["back_dir"].asString();
                _backup_file = root["backup_file"].asString();
                return true;
            }

        private:
            inline static Config *_instance = nullptr;
            inline static std::mutex _mtx;
            inline static std::string _default_config_file = "../src/Cloud.conf";
            // inline static std::string _default_config_file = "../project/src/Cloud.conf"; // for test
        };
    }
}