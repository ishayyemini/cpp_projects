#include <iostream>

#include "game/action.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    constexpr Action action = NONE;
    std::cout << action << std::endl;
    return 0;
}
