#include "main.h"
#include "./log/logger.hpp"

auto main() -> int
{
    std::unique_ptr<Main> test = std::make_unique<Main>();
    std::string hello = "Hello World!";
    lg("info",FileName(), Line(), "test: {}", hello);

    return 0;
}