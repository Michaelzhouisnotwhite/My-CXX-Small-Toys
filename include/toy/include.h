#ifndef _TOY_INCLUDE_H_
#define _TOY_INCLUDE_H_
#include "toy/pch.hpp"
#ifndef TIMEIT
#define TIMEIT toy::TimerWithRecorder _time(__PRETTY_FUNCTION__)
#endif

#ifndef TIMER_SESSION_BEGIN
#define TIMER_SESSION_BEGIN(...)                                               \
    toy::TimerRecorder::get().begin_session(__VA_ARGS__)
#endif
#ifndef DEBUG
#define DEBUG 1
#endif

#ifndef TIMER_VERBOSE
#define TIMER_VERBOSE 0
#endif
#ifndef TIMER_SESSION_END
#define TIMER_SESSION_END toy::TimerRecorder::get().end_session()
#endif
namespace fs = std::filesystem;
namespace ch = std::chrono;
namespace toy {
    template<typename... Args>
    void print_if_debug(Args &&...args) {
#if DEBUG
        fmt::println(std::forward<Args>(args)...);
#endif
    }

    std::filesystem::path GetExePath();
    struct profile_result_t {
        using hr_time_point = ch::high_resolution_clock::time_point;
        std::string name;
        hr_time_point start, end;
        std::uint64_t thread_id;
    };

    struct timer_session_t {
        std::string name;
        using u_ptr = std::unique_ptr<timer_session_t>;
    };
    class TimerRecorder {
    private:
        timer_session_t::u_ptr current_session_;
        std::fstream output_stream_;
        std::int64_t profile_count_;
        std::mutex write_lock_;

    public:
        TimerRecorder();

        void begin_session(
            const std::string &session_name,
            const std::string &filepath
            = (GetExePath() / fs::path("result.json")).string());
        void end_session();
        void dump_profile(const profile_result_t &result);
        void write_profile_header();

        void write_profile_footer();

        static TimerRecorder &get();
    };
    class TimerWithRecorder {
        using hr_clock = std::chrono::high_resolution_clock;

    public:
        auto get_thread_id_hash(const std::thread::id &id);

    public:
        explicit TimerWithRecorder(const char *timer_name);
        void Stop();
        ~TimerWithRecorder();

    private:
        std::hash<std::thread::id> thread_hash_func;
        hr_clock::time_point start_time_;
        std::string timer_name_;
    };

    // class TIMER_SESSION {
    // public:
    //     template<typename... Args>
    //     explicit TIMER_SESSION(Args &&...args) {
    //         BEGIN(std::forward<Args>(args)...);
    //     }
    //     template<typename... Args>
    //     static void BEGIN(Args &&...args) {
    //         toy::TimerRecorder::get().begin_session(
    //             std::forward<Args>(args)...);
    //     }
    //     static void END() {
    //         toy::TimerRecorder::get().end_session();
    //     }
    //     ~TIMER_SESSION() {
    //         END();
    //     }
    // };
    class ThreadPool {
        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> ready_tasks_;
        std::mutex queue_mtx_;
        std::condition_variable condition_;
        volatile bool thread_stop_ = false;

    public:
        std::uint64_t thread_count_ = 2;
        template<typename Func, typename... Args>
        using return_type = std::shared_future<
            typename std::invoke_result<Func, Args...>::type>;
        explicit ThreadPool(std::uint64_t thread_nums = 0) {
            if (thread_nums <= 0) {
#ifdef _WIN32
                SYSTEM_INFO sys_info;
                GetSystemInfo(&sys_info);
                thread_nums = sys_info.dwNumberOfProcessors;
#else
                thread_nums = get_nprocs();
#endif
            }
            thread_count_ = std::max(thread_nums, thread_count_);

            for (std::uint64_t i = 0; i < thread_count_; ++i) {
                workers_.emplace_back([this]() {
                    while (true) {
                        std::unique_lock<std::mutex> lk(
                            queue_mtx_);// 出作用域会自动释放锁
                        condition_.wait(lk, [this]() {
                            return !ready_tasks_.empty() || thread_stop_;
                        });
                        if (thread_stop_) {
                            return;
                        }
                        auto task = std::move(ready_tasks_.front());
                        ready_tasks_.pop();
                        lk.unlock();
                        task();
                    }
                });
            }
        }
        template<typename Func, typename... Args>
        return_type<Func, Args...> enqueue(Func &&_callback, Args &&...args) {
            using callback_return_type =
                typename std::result_of<Func(Args...)>::type;
            auto task
                = std::make_shared<std::packaged_task<callback_return_type()>>(
                    std::bind(
                        std::forward<Func>(_callback),
                        std::forward<Args>(args)...));
            std::unique_lock<std::mutex> lock(queue_mtx_);
            if (thread_stop_) {
                lock.unlock();
                throw std::runtime_error(fmt::format(
                    "Error in {}:{}\nEnqueue on stoped ThreadPool",
                    __FILE__,
                    __LINE__));
            }
            ready_tasks_.emplace([lmd_task = task]() {
                (*lmd_task)();
            });
            lock.unlock();
            condition_.notify_one();
            return task->get_future();
        }
        ~ThreadPool();
    };
}// namespace toy
#endif