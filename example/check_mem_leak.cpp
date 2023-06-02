#include "check_mem_leak.hpp"
#include <fmt/core.h>
#include <memory>


class Foo {
    int a;
};
int main(int args, char *argv[]) {
    { auto x = std::make_shared<Foo>(); }
    auto x = ALIGN(10);
    check_leaks();
    return 0;
}