#include <toy/include.h>

int main(int args, char *argv[]) {
    std::vector<std::string> strings{"I", "am", "from", "Earth"};
    toy::enumerate(strings, [](const auto i, const auto &value) {
        fmt::println("{}: {}", i, value);
        return false;
    });
    return 0;
}