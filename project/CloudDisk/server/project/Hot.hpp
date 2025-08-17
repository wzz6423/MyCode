#pragma once

// C++
#include <string>
#include <vector>
// C
#include <ctime>
// system call
#include <unistd.h>
// Other
#include "util.hpp"
#include "Data.hpp"

extern CloudBackUp::Data::DataManager *_data;

namespace CloudBackUp
{
    class HotManager
    {
    public:
        HotManager()
            : _back_dir(Config::Config::GetInstance()->GetBackDir()),
              _pack_dir(Config::Config::GetInstance()->GetPackDir()),
              _pack_suffix(Config::Config::GetInstance()->GetPackfileSuffix()),
              _hot_time(Config::Config::GetInstance()->GetHotTime())
        {
            Util::FileUtil tmp_p(_pack_dir);
            Util::FileUtil tmp_b(_back_dir);
            tmp_p.CreateDirectory();
            tmp_b.CreateDirectory();
        }

        bool Run()
        {
            while (true)
            {
                // 遍历备份目录, 获取所有文件名
                Util::FileUtil fu(_back_dir);
                std::vector<std::string> all;
                fu.ScanDirectory(&all);

                // 遍历判断文件是否为非热点文件
                for (auto &e : all)
                {
                    if (ColdFile(e))
                    {
                        // 获取文件备份信息
                        Data::BackupInfo info;
                        if (_data->Get1ByRealPath(e, &info) == false)
                        {
                            std::cerr << "backup info is not exists!" << std::endl;
                            info(e);
                        }

                        // 对非热点文件压缩处理
                        Util::FileUtil tmp(e);
                        tmp.Compress(info._pack_path);

                        // 删除原文件
                        tmp.Remove();
                        info._is_pack = true;

                        // 修改备份信息
                        _data->Update(info);
                    }
                }
                usleep(1000); // 避免空目录一直死循环检测消耗资源
            }

            return true;
        }

    private:
        // 非热点 - true, 热点 - false
        bool ColdFile(const std::string &filename)
        {
            Util::FileUtil fu(filename);
            return time(nullptr) - fu.LastAccessTime() > _hot_time;
        }

    private:
        std::string _back_dir;
        std::string _pack_dir;
        std::string _pack_suffix;
        int _hot_time;
    };
}