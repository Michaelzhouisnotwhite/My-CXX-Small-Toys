#include "toy/cpperclip.hpp"

namespace cpperclip = toy::cpperclip;
int main(int args, char *argv[]) {
    for (uint i = 0; i < 3; i++) {
        std::string res = cpperclip::wait_for_new_paste(3000);
        fmt::println("new paste: \n{}", res);
    }
    return 0;
}