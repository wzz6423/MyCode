#pragma once

// C++
#include <string>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <optional>
// system
// #include <pthread.h> // 读写锁
// Other
#include "util.hpp"
#include "Config.hpp"

/*
    C++17 提供了 shared_lock 配合 unique_lock 实现读写锁功能, 此处无需跨平台故使用 Linux 中 POSIX 的 pthread 库中的读写锁也可
*/

namespace CloudBackUp
{
    namespace Data
    {
        using BackupInfo = struct BackupInfo_t
        {
        public:
            BackupInfo_t() {}
            void operator()(const std::string &realPath)
            {
                New(realPath);
            }

            // 添加新对象的属性
            BackupInfo_t(const std::string &realPath)
            {
                New(realPath);
            }

        private:
            void New(const std::string &realPath)
            {
                Util::FileUtil fu(realPath);
                if (fu.Exists() == false)
                {
                    std::cerr << "BackupInfo: file is not exists!" << std::endl;
                    return;
                }
                std::optional<int64_t> fsize = fu.FileSize();

                Config::Config *cfg = Config::Config::GetInstance();
                std::string packDir = cfg->GetPackDir();
                std::string packSuffix = cfg->GetPackfileSuffix();

                std::string downloadPrefix = cfg->GetDownloadPrefix();

                _is_pack = false;
                if (fsize)
                {
                    _file_size = fsize.value();
                }
                else
                {
                    std::cerr << "BackupInfo: get file size error!" << std::endl;
                }
                _modify_time = fu.LastModifyTime();
                _access_time = fu.LastAccessTime();
                _real_path = realPath;
                _pack_path = packDir + fu.Filename() + packSuffix;
                _url = downloadPrefix + fu.Filename();
            }

        public:
            bool _is_pack;          // 压缩标志
            size_t _file_size;      // 文件大小
            time_t _modify_time;    // 最后一次修改时间
            time_t _access_time;    // 最后一次访问时间
            std::string _real_path; // 文件存储实际路径
            std::string _pack_path; // 文件压缩存储的路径
            std::string _url;       // url 中的访问路径
        };

        class DataManager
        {
        public:
            DataManager()
                : _backup_file(Config::Config::GetInstance()->GetBackupFile())
            {
                // 初始化读写锁
                // pthread_rwlock_init(&_rwlock, nullptr);

                // 初始化加载
                InitLoad();
            }
            // ~DataManager()
            // {
            //     // 销毁读写锁
            //     pthread_rwlock_destroy(&_rwlock);
            // }

            // 插入
            bool Insert(const BackupInfo info)
            {
                {
                    // pthread_rwlock_clockwrlock(&_rwlock);
                    std::unique_lock lock(_rwlock);
                    _table[info._url] = std::move(info);
                    // pthread_rwlock_unlock(&_rwlock);
                }
                Storage();
                return true;
            }

            // 更改
            bool Update(const BackupInfo info)
            {
                {
                    // pthread_rwlock_clockwrlock(&_rwlock);
                    std::unique_lock lock(_rwlock);
                    _table[info._url] = std::move(info);
                    // pthread_rwlock_unlock(&_rwlock);
                }
                Storage();
                return true;
            }

            // 查
            bool Get1ByURL(const std::string &url, BackupInfo *info)
            {
                {
                    // pthread_rwlock_rdlock(&_rwlock);
                    std::shared_lock lock(_rwlock);
                    std::unordered_map<std::string, BackupInfo>::iterator it = _table.find(url);
                    if (it != _table.end())
                    {
                        *info = it->second;
                        // pthread_rwlock_unlock(&_rwlock);
                        return true;
                    }
                    else
                    {
                        // pthread_rwlock_unlock(&_rwlock);
                        return false;
                    }
                }
            }
            bool Get1ByRealPath(const std::string &realPath, BackupInfo *info)
            {
                {
                    // pthread_rwlock_rdlock(&_rwlock);
                    std::shared_lock lock(_rwlock);
                    if (_table.empty())
                    {
                        // pthread_rwlock_unlock(&_rwlock);
                        return false;
                    }
                    for (auto &e : _table)
                    {
                        if (e.second._real_path == realPath)
                        {
                            *info = e.second;
                            // pthread_rwlock_unlock(&_rwlock);
                            return true;
                        }
                    }
                    // pthread_rwlock_unlock(&_rwlock);
                    return false;
                }
            }
            bool GetAll(std::vector<BackupInfo> *all)
            {
                {
                    // pthread_rwlock_rdlock(&_rwlock);
                    std::shared_lock lock(_rwlock);
                    if (_table.empty())
                    {
                        // pthread_rwlock_unlock(&_rwlock);
                        return false;
                    }
                    else
                    {
                        for (auto &e : _table)
                        {
                            all->push_back(e.second);
                        }
                        // pthread_rwlock_unlock(&_rwlock);
                        return true;
                    }
                }
            }

            // 持久化存储
            bool Storage()
            {
                // 获取所有数据
                std::vector<BackupInfo> all;
                if (GetAll(&all) == false)
                {
                    std::cerr << "Storage: get all error!" << std::endl;
                    return false;
                }

                // 添加到 Json::=Value
                Json::Value root;
                for (auto &e : all)
                {
                    Json::Value item;
                    item["is_pack"] = e._is_pack;
                    item["file_size"] = e._file_size;
                    item["modify_time"] = e._modify_time;
                    item["access_time"] = e._access_time;
                    item["real_path"] = e._real_path;
                    item["pack_path"] = e._pack_path;
                    item["url"] = e._url;
                    root.append(item);
                }

                // 序列化 Json:Value
                std::string body;
                Util::JsonUtil::Serialize(root, &body);

                // 写文件
                Util::FileUtil fu(_backup_file);
                fu.SetContent(body);
                return true;
            }

            // 从文件中初始化加载文件数据
            bool InitLoad()
            {
                // 打开文件读取出其中数据
                Util::FileUtil fu(_backup_file);
                if (fu.Exists() == false)
                {
                    std::cerr << "InitLoad: file is not exists error!" << std::endl;
                    return false;
                }
                std::string body;
                fu.GetContent(&body);

                // 反序列化
                Json::Value root;
                if (Util::JsonUtil::UnSerialize(body, &root) == false)
                {
                    std::cerr << "InitLoad: unserialize error!" << std::endl;
                    return false;
                }

                // 将 Json::value 中的数据添加在 _table 中
                for (int i = 0; i < root.size(); ++i)
                {
                    BackupInfo info;
                    info._is_pack = root[i]["is_pack"].asBool();
                    info._file_size = root[i]["file_size"].asInt64();
                    info._modify_time = root[i]["modify_time"].asInt64();
                    info._access_time = root[i]["access_time"].asInt64();
                    info._real_path = root[i]["real_path"].asString();
                    info._pack_path = root[i]["pack_path"].asString();
                    info._url = root[i]["url"].asString();
                    Insert(info);
                }
                return true;
            }

        private:
            // pthread_rwlock_t _rwlock;                        // 读写锁 -- 写互斥, 读共享
            mutable std::shared_mutex _rwlock;                  // C++ 的读写锁
            std::string _backup_file;                           // 数据持久化存储的文件
            std::unordered_map<std::string, BackupInfo> _table; // 哈希映射表, 映射实际存储路径和对应的文件数据结构体
        };
    }
}