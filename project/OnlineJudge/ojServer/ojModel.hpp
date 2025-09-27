#pragma once

// C++
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <ranges>
#include <algorithm>
#include <optional>
#include <format>
// C
#include <cstdlib>
// Other
#include "../common/utils.hpp"

namespace model
{
    struct Question
    {
        std::string number;      // 题目编号, 唯一
        std::string title;       // 题目标题
        std::string difficulty;  // 题目难度
        std::string description; // 题目描述
        int timeLimit;           // 时间限制
        int memoryLimit;         // 内存限制
        std::string head;        // 题目预设代码, 头部提供给用户的代码
        std::string tail;        // 题目测试用例, 尾部和用户提交代码组合交给编译运行模块进行测试的代码
    };

    class Model
    {
    public:
        Model()
        {
            if (!LoadQuestions(_questionsPath))
            {
                // 使用std::format生成结构化错误信息
                auto error_msg = std::format(
                    "加载问题失败: 无法从路径 '{}' 加载数据",
                    _questionsPath);

                // 输出错误信息
                LOG(FATAL) << error_msg << '\n';

                // 强制终止程序(与assert行为一致，但在所有编译模式下生效)
                std::terminate();
            }
        }
        ~Model() = default;

        // 加载题目列表
        auto LoadQuestions(const std::string &path) -> bool
        {
            // 加载配置文件 & 题目编号文件
            return utils::FileUtil::ProcessLines(path, [&](const std::string &line) -> void
                                                 {
                // 插入单个题目
                // 解析题目信息
                std::vector<std::string> tokens = utils::StringUtil::SplitNonEmpty(line, " ");
                if(tokens.size() != 5){
                    // 解析失败则跳过
                    LOG(ERROR) << "加载题目失败, 行: " << line << std::endl;
                }
                else
                {
                    std::shared_ptr<Question> q = std::make_shared<Question>();

                    // 填充题目信息
                    q->number = tokens[0];
                    q->title = tokens[1];
                    q->difficulty = tokens[2];
                    q->timeLimit = std::stoi(tokens[3]);
                    q->memoryLimit = std::stoi(tokens[4]);

                    // 构建题目相关文件路径
                    std::string headPath = _questionPath + q->number + "/head.cpp";
                    std::string tailPath = _questionPath + q->number + "/tail.cpp";
                    std::string descPath = _questionPath + q->number + "/desc.txt";
                    // 读取题目描述 & 头尾文件
                    q->description = utils::FileUtil::File(descPath).value_or("found description error");
                    q->head = utils::FileUtil::File(headPath).value_or("found head error");
                    q->tail = utils::FileUtil::File(tailPath).value_or("found tail error");

                    // 插入题目
                    _questions[q->number] = q;
                } });
        }

        // 获取所有题目列表
        auto GetAllQuestions() -> std::optional<std::vector<std::shared_ptr<Question>>>
        {
            if (_questions.empty())
            {
                LOG(ERROR) << "加载题目列表失败!" << std::endl;
                return std::nullopt;
            }
            else
            {
                // 最方便的方法: 使用std::ranges::to, 直接构造vector
                return std::make_optional(std::ranges::to<std::vector<std::shared_ptr<Question>>>(_questions | std::views::values));
            }
        }

        // 获取一个题目详细内容
        auto GetQuestion(const std::string &number) -> std::optional<std::shared_ptr<Question>>
        {
            const auto &e = _questions.find(number);
            if (e == _questions.end())
            {
                LOG(ERROR) << "题目编号不存在!" << std::endl;
                return std::nullopt;
            }
            else
            {
                return std::make_optional(e->second);
            }
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<Question>> _questions; // 题目信息映射

        inline static const std::string _questionsPath = "./questions/questions.list"; // 题目编号文件
        inline static const std::string _questionPath = "./questions/";                // 题目目录文件
    };
}