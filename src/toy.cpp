#include "pch.hpp"
#include "toy/include.h"
std::filesystem::path toy::GetExePath() {
#ifdef _WIN32// 如果是Windows平台
        char buffer[MAX_PATH] = {0};
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
#elif// 如果是Linux平台
        char buffer[PATH_MAX] = {0};
        ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
        if (count < 0 || count >= PATH_MAX) return "";
#endif
        return std::filesystem::path(buffer).parent_path();
}