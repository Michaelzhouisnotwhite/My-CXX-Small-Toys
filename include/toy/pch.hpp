#pragma once
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <variant>
#include <ios>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <future>
#include <functional>
#include <optional>



#ifdef _WIN32// 如果是Windows平台
#include <windows.h>
#else// 如果是Linux平台
#include <unistd.h>

#include <sys/sysinfo.h>

#endif
namespace toy {
    namespace fs = std::filesystem;
    namespace ch = std::chrono;
    using namespace std::chrono_literals;
    using namespace std::string_literals;
}// namespace toy
