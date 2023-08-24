#include "toy/include.h"
#include "private/pch.hpp"

#ifdef PATH_MAX
#undef PATH_MAX
#endif
#define PATH_MAX 1024
std::filesystem::path toy::GetExePath() {
#ifdef _WIN32// 如果是Windows平台
    char buffer[MAX_PATH] = {0};
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
#else// 如果是Linux平台
    char buffer[PATH_MAX] = {0};
    ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
    if (count < 0 || count >= PATH_MAX) return "";
#endif
    return std::filesystem::path(buffer).parent_path();
}

std::uint64_t toy::get_core_nums() {
#ifdef _WIN32
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    return sys_info.dwNumberOfProcessors;
#else
    return get_nprocs();
#endif
}
#undef PATH_MAX
