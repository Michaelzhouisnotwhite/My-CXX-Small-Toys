#include "toy/include.h"

int main(int args, char* argv[])
{
    for (auto i = 0ull; i < args; i++) {
        fmt::println(argv[i]);
    }
    fmt::println("exe path: {}", toy::GetExePath().string());
    return 0;
}