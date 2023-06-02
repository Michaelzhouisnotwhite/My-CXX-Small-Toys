#pragma once
#include <algorithm>
#include <bits/chrono.h>
#include <chrono>
#include <corecrt.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string_view>
#include <thread>
#include <fstream>

#ifdef _WIN32// 如果是Windows平台
#include <windows.h>
#elif// 如果是Linux平台
#include <unistd.h>
#endif
namespace toy {
    auto GetExePath() {
#ifdef _WIN32// 如果是Windows平台
        char buffer[MAX_PATH] = {0};
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
#elif// 如果是Linux平台
        char buffer[PATH_MAX] = {0};
        ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
        if (count < 0 || count >= PATH_MAX) return "";
#endif
        std::filesystem::path path(buffer);
        return path.parent_path().string();
    }
}// namespace toy
namespace fs = std::filesystem;
namespace ch = std::chrono;
using namespace std::chrono_literals;
using namespace std::string_literals;