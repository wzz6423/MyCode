import math;
#include <print>

int main()
{
    int a = 10;
    int b = 5;

    int sum = add(a, b);
    int difference = sub(a, b);

    std::println("Sum: {}", sum);
    std::println("Difference: {}", difference);

    // Print();

    return 0;
}

// cmake -G Ninja ..
// cmake --build . -j 1