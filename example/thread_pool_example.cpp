#include "pch.h"
#include "toy/include.h"

using namespace std::literals;
static const std::string &url1 = "https://juejin.cn/post/7133062775487594532"s;
static const std::string &url2
    = "https://blog.csdn.net/anjisi/article/details/53899222#comments_18432585"s;
auto print_resp(cpr::Response resp) {
    std::puts("------------------------------");
    fmt::println("url: {}", resp.url.str());
    fmt::println("Content-Type: {}", resp.header["Content-Type"]);
    fmt::println("Status Code: {}", resp.status_code);
}
auto wait_for(std::uint32_t seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
    return fmt::format("sleep for {}s"s, seconds);
}
auto test_http_without_threadpool() {
    TIMEIT;
    auto resp1 = cpr::Get(cpr::Url{::url1});
    auto resp2 = cpr::Get(cpr::Url(::url2));
    print_resp(resp1);
    print_resp(resp2);
}
auto test_http_with_threadpool() {
    toy::ThreadPool pool;
    {
        TIMEIT;
        auto future_1 = pool.enqueue([]() {
            return cpr::Get(cpr::Url{::url1});;
        });

        auto future_2 = pool.enqueue([]() {
            return cpr::Get(cpr::Url(::url2));
        });

    ::print_resp(future_1.get());
    ::print_resp(future_2.get());
    }
}

int main() {
    TIMER_SESSION_BEGIN("thread pool compare", "./.timer_test_result.json");
    test_http_with_threadpool();
    test_http_without_threadpool();
    TIMER_SESSION_END;
    return 0;
}