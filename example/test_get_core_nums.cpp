#include "toy/include.h"
#include <iostream>


int main(int args, char *argv[]) {
    std::cout << "Core Numbers: " << toy::get_core_nums() << std::endl;
    return 0;
}