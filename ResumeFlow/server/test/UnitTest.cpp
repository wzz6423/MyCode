/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test util.hpp 1 -- file_util
// 测试通过
// #include <print>
// #include "../project/util.hpp"
// auto main(int argc, char* argv[]) -> int
// {
//     std::string filename = argv[1];
//     CloudBackUp::Util::FileUtil fu(filename);
//     std::println("file name: {}, file size: {}, last modify time: {}, last access time: {}",
//     fu.Filename(), fu.FileSize().value(), fu.LastModifyTime(), fu.LastAccessTime());

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test util.hpp 2 -- file_util
// 测试通过
// #include <print>
// #include "../project/util.hpp"
// auto main(int argc, char* argv[]) -> int
// {
//     std::string filename = argv[1];
//     CloudBackUp::Util::FileUtil fu(filename);
//     std::string body;
//     fu.GetContent(&body);

//     CloudBackUp::Util::FileUtil fu2("./text.txt");
//     fu2.SetContent(body);

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test util.hpp 3 -- file_util
// 测试通过
// #include <print>
// #include "../project/util.hpp"
// auto main(int argc, char* argv[]) -> int
// {
//     std::string filename = argv[1];
//     std::string package = filename + ".lz";
//     CloudBackUp::Util::FileUtil fu(filename);
//     std::string body;
//     fu.Compress(package);

//     CloudBackUp::Util::FileUtil fu2(package);
//     fu2.UnCompress("./" + filename + ".2");

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test util.hpp 4 -- file_util
// 测试通过
// #include <print>
// #include "../project/util.hpp"
// auto main(int argc, char *argv[]) -> int
// {
//     std::string filename = argv[1];
//     CloudBackUp::Util::FileUtil fu(filename);
//     fu.CreateDirectory();
//     std::vector<std::string> arry;
//     fu.ScanDirectory(&arry);
//     for (auto &e : arry)
//     {
//         std::println("{}",e);
//     }

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test util.hpp 5 -- json_util
// 测试通过
// #include <print>
// #include "../project/util.hpp"
// auto main(int argc, char* argv[]) -> int
// {
//     const char* name = "小明";
//     int age = 19;
//     float score[] = {99.99,88.88,77.77};
//     Json::Value root;
//     root["姓名"] = name;
//     root["年龄"] = age;
//     root["成绩"].append(score[0]);
//     root["成绩"].append(score[1]);
//     root["成绩"].append(score[2]);
//     std::string str;
//     CloudBackUp::Util::JsonUtil::Serialize(root, &str);
//     std::println("{}", str);
//     std::println("----------------------------------------");
//     Json::Value value;
//     CloudBackUp::Util::JsonUtil::UnSerialize(str, &value);
//     std::println("姓名: {}, 年龄: {}, 成绩: {}、{}、{}", value["姓名"].asString(), value["年龄"].asInt(),
//                 value["成绩"][0].asFloat(),value["成绩"][1].asFloat(),value["成绩"][2].asFloat());

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test configt.hpp
// 测试通过
// #include <print>
// #include "../project/Config.hpp"
// auto main() -> int
// {
//     CloudBackUp::Config::Config* cfg = CloudBackUp::Config::Config::GetInstance();
//     std::println("hot time: {}, server port: {}, server ip: {}, download prefix: {}, pack file suffixe: {}, pack dir: {}, back dir: {}, backup file: {}",
//         cfg->GetHotTime(), cfg->GetServerPort(), cfg->GetServerIp(), cfg->GetDownloadPrefix(),
//         cfg->GetPackfileSuffix(), cfg->GetPackDir(), cfg->GetBackDir(), cfg->GetBackupFile());

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test data.hpp 1
// 测试通过
// #include <print>
// #include "../project/Data.hpp"
// auto main(int arhc, char *argv[]) -> int
// {
//     std::string filename = argv[1];
//     CloudBackUp::Data::BackupInfo info(filename);
//     std::println("_is_pack: {}, _file_size: {}, _modify_time: {}, _access_time: {}, _real_path: {}, _pack_path: {}, _url: {}",
//     info._is_pack, info._file_size, info._modify_time, info._access_time, info._real_path, info._pack_path, info._url);

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test data.hpp 2
// 测试通过
// #include <vector>
// #include <print>
// #include "../project/Data.hpp"
// auto main() -> int
// {
//     std::string filename = "../project/util.hpp";
//     CloudBackUp::Data::BackupInfo info(filename);
//     CloudBackUp::Data::DataManager manager;

//     CloudBackUp::Data::BackupInfo ret1;
//     CloudBackUp::Data::BackupInfo ret2;
//     std::vector<CloudBackUp::Data::BackupInfo> ret3;

//     manager.Insert(info);
//     manager.Get1ByURL(info._url, &ret1);
//     info._is_pack = true;
//     manager.Update(info);
//     manager.Get1ByRealPath(info._real_path, &ret2);
//     manager.GetAll(&ret3);
//     std::println("ret1: _is_pack: {}, _file_size: {}, _modify_time: {}, _access_time: {}, _real_path: {}, _pack_path: {}, _url: {}",
//                  ret1._is_pack, ret1._file_size, ret1._modify_time, ret1._access_time, ret1._real_path, ret1._pack_path, ret1._url);
//     std::println("ret2: _is_pack: {}, _file_size: {}, _modify_time: {}, _access_time: {}, _real_path: {}, _pack_path: {}, _url: {}",
//                  ret2._is_pack, ret2._file_size, ret2._modify_time, ret2._access_time, ret2._real_path, ret2._pack_path, ret2._url);
//     int i = 0;
//     for (auto &e : ret3)
//     {
//         ++i;
//         std::println("ret3-{}: _is_pack: {}, _file_size: {}, _modify_time: {}, _access_time: {}, _real_path: {}, _pack_path: {}, _url: {}", i,
//                      e._is_pack, e._file_size, e._modify_time, e._access_time, e._real_path, e._pack_path, e._url);
//     }

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test data.hpp 3
// 测试通过
// #include <vector>
// #include <print>
// #include "../project/Data.hpp"
// auto main() -> int
// {
//     std::string filename = "../project/util.hpp";
//     CloudBackUp::Data::BackupInfo info(filename);
//     CloudBackUp::Data::DataManager manager;

//     CloudBackUp::Data::BackupInfo ret1;
//     CloudBackUp::Data::BackupInfo ret2;
//     std::vector<CloudBackUp::Data::BackupInfo> ret3;

//     manager.Insert(info);
//     manager.Get1ByURL(info._url, &ret1);
//     info._is_pack = true;
//     manager.Update(info);
//     manager.Get1ByRealPath(info._real_path, &ret2);
//     manager.GetAll(&ret3);
//     std::println("ret1: _is_pack: {}, _file_size: {}, _modify_time: {}, _access_time: {}, _real_path: {}, _pack_path: {}, _url: {}",
//                  ret1._is_pack, ret1._file_size, ret1._modify_time, ret1._access_time, ret1._real_path, ret1._pack_path, ret1._url);
//     std::println("ret2: _is_pack: {}, _file_size: {}, _modify_time: {}, _access_time: {}, _real_path: {}, _pack_path: {}, _url: {}",
//                  ret2._is_pack, ret2._file_size, ret2._modify_time, ret2._access_time, ret2._real_path, ret2._pack_path, ret2._url);
//     int i = 0;
//     for (auto &e : ret3)
//     {
//         ++i;
//         std::println("ret3-{}: _is_pack: {}, _file_size: {}, _modify_time: {}, _access_time: {}, _real_path: {}, _pack_path: {}, _url: {}", i,
//                      e._is_pack, e._file_size, e._modify_time, e._access_time, e._real_path, e._pack_path, e._url);
//     }

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test data.hpp 4
// 测试通过
// #include <vector>
// #include <print>
// #include "../project/Data.hpp"
// 先执行, 产生存储文件
// auto main() -> int
// {
//     std::string filename = "../project/util.hpp";
//     CloudBackUp::Data::BackupInfo info(filename);
//     CloudBackUp::Data::DataManager manager;

//     CloudBackUp::Data::BackupInfo ret1;
//     CloudBackUp::Data::BackupInfo ret2;
//     std::vector<CloudBackUp::Data::BackupInfo> ret3;

//     manager.Insert(info);
//     manager.Get1ByURL(info._url, &ret1);
//     info._is_pack = true;
//     manager.Update(info);
//     manager.Get1ByRealPath(info._real_path, &ret2);
//     manager.GetAll(&ret3);
//     std::println("ret1: _is_pack: {}, _file_size: {}, _modify_time: {}, _access_time: {}, _real_path: {}, _pack_path: {}, _url: {}",
//                  ret1._is_pack, ret1._file_size, ret1._modify_time, ret1._access_time, ret1._real_path, ret1._pack_path, ret1._url);
//     std::println("ret2: _is_pack: {}, _file_size: {}, _modify_time: {}, _access_time: {}, _real_path: {}, _pack_path: {}, _url: {}",
//                  ret2._is_pack, ret2._file_size, ret2._modify_time, ret2._access_time, ret2._real_path, ret2._pack_path, ret2._url);
//     int i = 0;
//     for (auto &e : ret3)
//     {
//         ++i;
//         std::println("ret3-{}: _is_pack: {}, _file_size: {}, _modify_time: {}, _access_time: {}, _real_path: {}, _pack_path: {}, _url: {}", i,
//                      e._is_pack, e._file_size, e._modify_time, e._access_time, e._real_path, e._pack_path, e._url);
//     }

//     return 0;
// }
// 再执行, 测试读取缓存
// auto main() -> int
// {
//     CloudBackUp::Data::DataManager manager;
//     std::vector<CloudBackUp::Data::BackupInfo> ret3;
//     manager.GetAll(&ret3);
//     int i = 0;
//     for (auto &e : ret3)
//     {
//         ++i;
//         std::println("ret3-{}: _is_pack: {}, _file_size: {}, _modify_time: {}, _access_time: {}, _real_path: {}, _pack_path: {}, _url: {}", i,
//                      e._is_pack, e._file_size, e._modify_time, e._access_time, e._real_path, e._pack_path, e._url);
//     }

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test hot.hpp
// 测试通过
// bash:
// mkdir backdir
// cp ../project/bundle/bundle.cpp ./backdir/
// #include <print>
// #include "../project/Hot.hpp"
// CloudBackUp::Data::DataManager* _data;
// auto main() -> int
// {
//     _data = new CloudBackUp::Data::DataManager();
//     CloudBackUp::HotManager hot;
//     hot.Run();

//     delete _data;

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test server.hpp
// 测试通过
// #include <print>
// #include <thread>
// #include "../project/Server.hpp"
// CloudBackUp::Data::DataManager *_data;
// auto main() -> int
// {
//     _data = new CloudBackUp::Data::DataManager();
//     CloudBackUp::Server::Server srv;
//     while(true){
//         srv.Run();
//     }

//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// test all server
// 测试通过
#include <thread>
#include "../project/server.h"
CloudBackUp::Data::DataManager *_data;
void HotManager()
{
    CloudBackUp::HotManager hot;
    hot.Run();
}
void Server()
{
    CloudBackUp::Server::Server srv;
    srv.Run();
}
auto main() -> int
{
    _data = new CloudBackUp::Data::DataManager();
    std::thread hotManager(HotManager);
    std::thread serverManager(Server);

    hotManager.join();
    serverManager.join();

    return 0;
}