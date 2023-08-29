#include "toy/cpperclip.hpp"

namespace cpperclip = toy::cpperclip;
int main(int args, char *argv[]) {
    try {

        for (int i = 0; i < 3; i++) {
            std::string res = cpperclip::wait_for_new_paste(3000);
            fmt::println("new paste: \n{}", res);
        }
    } catch (std::exception &e) {
        fmt::println(e.what());
    }
    
    while (true) {
        std::string res = cpperclip::wait_for_new_paste();
        fmt::println("new paste: \n{}", res);
    }
    return 0;
}