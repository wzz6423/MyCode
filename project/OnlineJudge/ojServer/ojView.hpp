#pragma once

// C++
#include <iostream>
#include <string>
#include <vector>
// Other
#include <ctemplate/template.h>
#include "ojModel.hpp"
#include "../common/log.hpp"

namespace view
{
    class View
    {
    public:
        View() = default;
        ~View() = default;

        // 渲染题目列表 html 网页
        auto ExpandQuestions(const std::vector<std::shared_ptr<model::Question>> &questions) -> std::string
        {
            // 生成字典
            // 题目编号 题目标题 题目难度
            ctemplate::TemplateDictionary questionsDict("questions");
            for (const auto &q : questions)
            {
                ctemplate::TemplateDictionary *question = questionsDict.AddSectionDictionary("question");
                question->SetValue("number", q->number);
                question->SetValue("title", q->title);
                question->SetValue("difficulty", q->difficulty);
            }

            // 获取并渲染 html, 表格显示题目列表
            std::string html;
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(questionsHtml, ctemplate::DO_NOT_STRIP);
            tpl->Expand(&html, &questionsDict);
            return std::move(html);
        }

        // 渲染题目 html 网页
        auto ExpandQuestion(const std::shared_ptr<model::Question> &question) -> std::string
        {
            // 渲染单个题目页面
            // 生成字典
            // 题目编号 题目标题 题目难度 题目描述 预设代码
            ctemplate::TemplateDictionary questionDict("question");
            questionDict.SetValue("number", question->number);
            questionDict.SetValue("title", question->title);
            questionDict.SetValue("difficulty", question->difficulty);
            questionDict.SetValue("description", question->description);
            questionDict.SetValue("pre_code", question->head);

            // 获取并渲染 html, 显示题目详情
            std::string html;
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(questionHtml, ctemplate::DO_NOT_STRIP);
            tpl->Expand(&html, &questionDict);
            return std::move(html);
        }

    private:
        inline static const std::string templatePath = "./template/";                    // 待渲染 html 文件路径
        inline static const std::string questionsHtml = templatePath + "questions.html"; // 题目列表 html 文件路径
        inline static const std::string questionHtml = templatePath + "question.html";   // 题目 html 文件路径
    };
}