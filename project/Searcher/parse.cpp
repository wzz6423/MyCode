#include "parse.hpp"

auto main() -> int
{
    std::vector<std::string> htmlFiles;
    // 递归将每个 路径+文件名 形式的 html 文件信息保存到 htmlFiles 中以便于文件读取
    if (!parse::EnumFiles(parse::srcPath, &htmlFiles))
    {
        std::println("EnumFiles failed");
        return 1;
    }

    // 按照 htmlFiles 存放的数据读取每个文件的内容并进行解析去标签
    std::vector<parse::HtmlInfo> htmlInfos;
    if (!parse::ParseHtml(htmlFiles, &htmlInfos))
    {
        std::println("ParseHtml failed");
        return 2;
    }

    // 解析完成后将 htmlInfos 中的数据写入到 rswPath 中, 按照 \n 作为每个文档的分隔符
    if (!parse::SaveHtmlInfos(htmlInfos, parse::rswPath))
    {
        std::println("SaveHtmlInfos failed");
        return 3;
    }

    return 0;
}