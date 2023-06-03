#ifndef _TOY_INCLUDE_H_
#define _TOY_INCLUDE_H_
#include "toy/pch.hpp"
#ifndef TIMEIT
#define TIMEIT toy::TimerWithRecorder _time(__PRETTY_FUNCTION__)
#endif

#ifndef TIMER_SESSION_BEGIN
#define TIMER_SESSION_BEGIN(X) toy::TimerRecorder::get().begin_session(X)
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
}// namespace toy
#endif