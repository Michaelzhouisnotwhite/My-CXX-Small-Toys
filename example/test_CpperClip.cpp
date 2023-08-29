#include "pch.h"
#include "toy/cpperclip.hpp"

using namespace std::literals;
int main(int args, char *argv[]) {
    // while (true) {
    auto ins = toy::cpperclip::CpperClip([](const std::string &result) {
        fmt::println("{}", result);
    });
    std::this_thread::sleep_for(4s);
    ins.stop();
    // return 0;
    // }

    return 0;
}