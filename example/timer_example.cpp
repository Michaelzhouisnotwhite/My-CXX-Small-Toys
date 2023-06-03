#include "toy/include.h"
#include <filesystem>
#include <thread>
#include <variant>
#include <iostream>
#include <chrono>
namespace fs = std::filesystem;
class A {
public:
};
auto test_func() {
    TIMEIT;
    uint64_t value = 0;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    for (uint64_t i = 0; i < 4000000; ++i) {
        value += i;
    }
}

int main(int args, char *argv[]) {
    TIMER_SESSION_BEGIN("timer test");
    test_func();
    TIMER_SESSION_END;
    std::cout << toy::GetExePath().string();
    return 0;
}