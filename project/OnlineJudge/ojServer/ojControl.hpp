#pragma once

// C++
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <atomic>
#include <queue>
#include <mutex>
// Other
#include "ojModel.hpp"
#include "ojView.hpp"
#include "../common/httplib.h"
#include "../common/log.hpp"
#include "../common/utils.hpp"

namespace control
{
    /**
     * @brief 表示提供判题服务的主机节点
     * 存储主机的网络信息、负载情况和在线状态
     */
    class Machine
    {
    public:
        Machine(const std::string &ip = "", int port = 0, uint64_t id = 0)
            : _ip(ip), _port(port), _load(0), _id(id), _online(true)
        {
        }

        ~Machine() = default;

        /**
         * @brief 获取主机IP地址
         * @return 主机IP地址字符串
         */
        auto Ip() const -> std::string { return _ip; }

        /**
         * @brief 获取主机端口号
         * @return 主机端口号
         */
        auto Port() const -> int { return _port; }

        /**
         * @brief 获取主机当前负载
         * @return 主机负载值(原子操作保证线程安全)
         */
        auto Load() const -> uint64_t { return _load.load(); }

        /**
         * @brief 设置主机负载
         * @param load 新的负载值(原子操作保证线程安全)
         */
        auto Load(uint64_t load) -> void { _load.store(load); }

        /**
         * @brief 增加主机负载
         * 原子操作保证线程安全
         * @param delta 负载增加量
         */
        auto AddLoad(uint64_t delta) -> void { _load.fetch_add(delta); }

        /**
         * @brief 减少主机负载
         * 原子操作保证线程安全
         * @param delta 负载减少量
         */
        auto SubLoad(uint64_t delta) -> void { _load.fetch_sub(delta); }

        /**
         * @brief 清空主机负载
         * 原子操作保证线程安全
         * @param delta 负载减少量
         */
        auto ClearLoad() -> void { _load.store(0); }

        /**
         * @brief 获取主机唯一标识符
         * @return 主机ID
         */
        auto Id() const -> uint64_t { return _id; }

        /**
         * @brief 检查主机是否在线
         * @return 在线状态(true为在线, false为离线)
         */
        auto IsOnline() const -> bool { return _online; }

        /**
         * @brief 设置主机在线状态
         * @param online 新的在线状态(true为在线, false为离线)
         */
        auto SetOnline(bool online) -> void { _online = online; }

    private:
        std::string _ip;             // 主机IP地址
        int _port;                   // 主机端口号
        std::atomic<uint64_t> _load; // 主机负载(原子类型确保多线程安全)
        uint64_t _id;                // 主机唯一标识符
        bool _online;                // 主机在线状态标记
    };

    class LoadBalance
    {
    public:
        /**
         * @brief 构造函数
         * 初始化时加载主机配置文件, 加载失败则终止程序
         */
        LoadBalance()
        {
            if (!LoadConf(_machine_config_path))
            {
                LOG(FATAL) << "加载主机配置失败: " << _machine_config_path << std::endl;
                std::terminate();
            }
        }

        /**
         * @brief 加载主机配置文件并注册主机
         * 解析配置文件中的每行主机信息(IP:端口), 注册到优先队列
         * @param conf_path 配置文件路径
         * @return 加载成功返回true, 否则返回false
         */
        auto LoadConf(const std::string &conf_path) -> bool
        {
            return utils::FileUtil::ProcessLines(conf_path, [&](const std::string &line)
                                                 {
                auto parts = utils::StringUtil::SplitNonEmpty(line, ":");
                if (parts.size() == 2)
                {
                    std::string ip = parts[0];
                    int port = std::stoi(parts[1]);
                    registerMachine(ip, port);
                }
                else
                {
                    LOG(ERROR) << "加载主机失败: " << line << std::endl;
                } });
        }

        /**
         * @brief 选择一个可用的主机(在线状态)并增加其负载
         * @return 若找到可用主机, 返回包含主机ID和主机指针的std::optional<std::tuple>；
         *         若无可用主机, 返回空的std::optional, 并将所有不可用主机设置为在线
         */
        auto SelectMachine() -> std::optional<std::tuple<int, std::shared_ptr<Machine>>>
        {
            // 加锁保证线程安全, 防止多线程同时操作_machines队列导致数据不一致
            std::lock_guard<std::mutex> lock(_mtx);

            // 临时存储从队列中取出的不在线主机, 后续需要放回队列
            std::vector<std::shared_ptr<Machine>> temp;
            // 存储最终结果, 默认为空(无可用主机)
            std::optional<std::tuple<int, std::shared_ptr<Machine>>> result = std::nullopt;

            // 循环从主机队列中获取主机, 直到队列为空
            while (!_machines.empty())
            {
                // 获取队列顶部的主机(优先级最高的主机)
                auto top = _machines.top();
                // 将该主机从队列中移除(后续需根据状态决定是否放回)
                _machines.pop();

                // 检查主机是否在线
                if (top->IsOnline())
                {
                    // 若在线, 增加其负载(当前任务分配给该主机)
                    top->AddLoad(1);
                    // 构造结果：主机ID(转换为int)和主机指针
                    result = std::make_tuple(static_cast<int>(top->Id()), top);
                    // 将当前选中的主机放回队列(仍需参与后续任务分配)
                    _machines.push(top);

                    // 将临时存储的不在线主机全部放回队列
                    for (const auto &m : temp)
                    {
                        _machines.push(m);
                    }
                    // 找到可用主机, 跳出循环
                    break;
                }
                else
                {
                    // 若主机不在线, 放入临时容器暂存
                    temp.push_back(top);
                }
            }

            // 若队列已空但临时容器中有主机(说明所有主机都被取出检查过)
            if (_machines.empty() && !temp.empty())
            {
                // 将所有临时存储的主机放回队列(恢复队列状态)
                for (const auto &m : temp)
                {
                    _machines.push(m);
                }
            }

            // 若未找到可用主机(result为空)
            if (!result.has_value())
            {
                // 将所有不在线的主机(存储在temp中)设置为在线状态
                for (const auto &m : temp)
                {
                    // 调用Online接口激活主机, 传入主机ID
                    Online(m->Id());
                }

                // 日志记录无可用主机(此时已尝试激活所有主机)
                LOG(ERROR) << "无可用主机, 已尝试将所有主机设置为在线状态" << std::endl;
            }

            // 返回结果(可能为空或包含选中的主机)
            return result;
        }

        /**
         * @brief 将指定ID的主机设置为在线状态
         * @param id 主机唯一标识符
         * @return 操作成功返回true, 主机不存在返回false
         */
        auto Online(uint64_t id) -> bool
        {
            return setMachineStatus(id, true);
        }

        /**
         * @brief 将指定ID的主机设置为离线状态
         * @param id 主机唯一标识符
         * @return 操作成功返回true, 主机不存在返回false
         */
        auto Offline(uint64_t id) -> bool
        {
            return setMachineStatus(id, false);
        }

    private:
        /**
         * @brief 注册新主机
         * 创建主机对象并添加到优先队列, 分配唯一ID, 确保线程安全
         * @param ip 主机IP地址
         * @param port 主机端口号
         */
        auto registerMachine(const std::string &ip, int port) -> void
        {
            std::lock_guard<std::mutex> lock(_mtx);
            auto machine = std::make_shared<Machine>(ip, port, _next_id++);
            _machines.push(machine);
            LOG(INFO) << "注册新主机: " << ip << ":" << port << " ID: " << (_next_id - 1);
        }

        /**
         * @brief 设置主机在线状态(私有辅助函数)
         * 遍历队列找到指定ID的主机并更新其在线状态
         * @param id 主机唯一标识符
         * @param online 新的在线状态
         * @return 操作成功返回true, 主机不存在返回false
         */
        auto setMachineStatus(uint64_t id, bool online) -> bool
        {
            std::lock_guard<std::mutex> lock(_mtx);
            std::vector<std::shared_ptr<Machine>> temp;
            bool found = false;

            while (!_machines.empty())
            {
                auto top = _machines.top();
                _machines.pop();

                if (top->Id() == id)
                {
                    top->SetOnline(online);
                    found = true;
                    temp.push_back(top);
                    if (!online)
                    {
                        top->ClearLoad(); // 下线时清空负载
                    }
                    LOG(INFO) << "主机 " << id << (online ? " 上线" : " 下线") << std::endl;
                    break;
                }
                else
                {
                    temp.push_back(top);
                }
            }

            while (!_machines.empty())
            {
                temp.push_back(_machines.top());
                _machines.pop();
            }

            for (const auto &m : temp)
            {
                _machines.push(m);
            }

            if (!found)
            {
                LOG(ERROR) << "主机 " << id << " 不存在" << std::endl;
            }

            return found;
        }

        // 声明比较器
        struct machineComparator
        {
            bool operator()(const std::shared_ptr<control::Machine> &a,
                            const std::shared_ptr<control::Machine> &b) const
            {
                return a->Load() > b->Load();
            }
        };

    private:
        // 优先队列存储主机, 按负载升序排列(小根堆)
        std::priority_queue<
            std::shared_ptr<Machine>,
            std::vector<std::shared_ptr<Machine>>,
            machineComparator>
            _machines;

        std::mutex _mtx;       // 保护共享资源的互斥锁
        uint64_t _next_id = 0; // 用于分配主机ID的计数器

        inline static const std::string _machine_config_path = "./conf/serverMachine.conf"; // 工作主机 ip:port 配置文件
    };

    /**
     * @brief 控制层类
     * 协调数据模型(Model)和视图(View), 处理业务逻辑, 调用负载均衡器分配任务
     */
    class Control
    {
    public:
        Control()
            : _model(std::make_unique<model::Model>()),
              _view(std::make_unique<view::View>()),
              _load_balance(std::make_unique<LoadBalance>())
        {
        }

        /**
         * @brief 获取所有题目的列表
         * 从数据模型获取题目列表, 通过视图层格式化后返回
         * @return 格式化后的题目列表字符串(通常为JSON格式)
         */
        auto AllQuestions() -> std::string
        {
            std::optional<std::vector<std::shared_ptr<model::Question>>> all_opt = _model->GetAllQuestions();
            if (all_opt.has_value())
            {
                std::vector<std::shared_ptr<model::Question>> all = std::move(all_opt.value());
                std::sort(all.begin(), all.end(), [](const auto &a, const auto &b)
                          { return std::stoi(a->number) < std::stoi(b->number); });
                return _view->ExpandQuestions(all);
            }
            else
            {
                return "获取题目列表失败";
            }
        }

        /**
         * @brief 根据题目编号获取题目详情
         * 从数据模型获取指定题目的详情, 通过视图层格式化后返回
         * @param number 题目编号
         * @return 格式化后的题目详情字符串(通常为JSON格式)
         */
        auto Question(const std::string &number) -> std::string
        {
            std::optional<std::shared_ptr<model::Question>> question_opt = _model->GetQuestion(number);
            if (question_opt.has_value())
            {
                return _view->ExpandQuestion(question_opt.value());
            }
            else
            {
                return "获取题目失败";
            }
        }

        /**
         * @brief 处理用户提交的代码判题请求
         * 解析用户请求, 获取题目信息, 通过负载均衡器选择主机, 发送判题请求并返回结果
         * @param inJson 包含题目编号和用户代码的JSON字符串
         * @return 判题结果字符串(通常为JSON格式)
         */
        auto Judge(const std::string &number, const std::string &inJson) -> std::string
        {
            // 根据题目编号读取题目细节(head, tail)
            std::optional<std::shared_ptr<model::Question>> question_opt = _model->GetQuestion(number);
            if (!question_opt.has_value())
            {
                return "获取题目失败";
            }
            std::shared_ptr<model::Question> question = question_opt.value();

            // 反序列化 inJSon
            // 提取信息 -- 题目id, 用户提交的源代码
            std::optional<Json::Value> inValue_opt = utils::JsonUtil::UnSerialize(inJson);
            if (!inValue_opt.has_value())
            {
                return "反序列化失败";
            }
            Json::Value inValue = std::move(inValue_opt.value());

            // 拼接用户提交的代码 & 测试用例形成新的代码
            Json::Value compileValue;
            compileValue["input"] = inValue["input"].asString();
            compileValue["code"] = inValue["code"].asString() + R"(\n\n)" + question->tail;
            compileValue["timeLimit"] = question->timeLimit;
            compileValue["memoryLimit"] = question->memoryLimit;
            std::optional<std::string> compileJson_opt = utils::JsonUtil::Serialize(compileValue);
            if (!compileJson_opt.has_value())
            {
                return "序列化失败";
            }
            std::string compileJson = std::move(compileJson_opt.value());

            // 负载均衡 -- 选择负载最低的主机
            // 一直选择, 直到找到一个在线的主机, 除非所有主机都下线了
            std::tuple<int, std::shared_ptr<control::Machine>> machine_tuple;
            while (true)
            {
                std::optional<std::tuple<int, std::shared_ptr<control::Machine>>> machine_opt =
                    _load_balance->SelectMachine();
                if (machine_opt.has_value())
                {
                    machine_tuple = std::move(machine_opt.value());
                    break;
                }
                else
                {
                    LOG(ERROR) << "所有主机都下线了" << std::endl;
                    return "所有主机都下线了";
                }
            }
            auto [id, machine] = std::move(machine_tuple);

            // 发起 http 请求, 得到运行结果
            httplib::Client cli(machine->Ip(), machine->Port());

            auto res = cli.Post("/compileRun", compileJson, "application/json;charset=utf-8");
            if (!res || res->status != 200)
            {
                LOG(ERROR) << "发起判题请求失败" << std::endl;
                machine->SubLoad(1); // 减少负载
                _load_balance->Offline(id);
                return "发起判题请求失败";
            }
            machine->SubLoad(1); // 减少负载

            // 返回测试结果
            return std::move(res->body);
        }

    private:
        std::unique_ptr<model::Model> _model;       // 数据模型
        std::unique_ptr<view::View> _view;          // 视图层
        std::unique_ptr<LoadBalance> _load_balance; // 负载均衡器(使用智能指针管理)
    };
}
