// 测试通过

#include "../../../project/common/utils/utils.hpp"

auto main() -> int
{
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << stellar_post::Utils::Uuid() << std::endl;
    }

    return 0;
}