#include "pch.hpp"
#include "toy/include.h"
#include <string_view>

using namespace std::chrono_literals;

namespace toy {

    void TimerRecorder::begin_session(
        const std::string &session_name, const std::string &filepath) {
        output_stream_.open(filepath, std::ios::out);
        current_session_
            = std::make_unique<timer_session_t>(timer_session_t{session_name});
        this->write_profile_header();
    }
    TimerRecorder::TimerRecorder()
        : current_session_(nullptr), profile_count_(0) {
    }
    void TimerRecorder::end_session() {
        this->write_profile_footer();
        output_stream_.close();
        current_session_.reset();
        profile_count_ = 0;
    }
    void TimerRecorder::dump_profile(const profile_result_t &result) {
        if (profile_count_ > 0) {
            output_stream_ << ",";
        }
        profile_count_++;
        std::string name = result.name;
        std::replace(name.begin(), name.end(), '"', '\'');
        std::unique_lock<std::mutex> lock(write_lock_);
        output_stream_ << fmt::format(
            R"({{
            "cat": "function",
            "dur": {},
            "name": "{}",
            "ph": "X",
            "pid": 0,
            "tid": {},
            "ts": {}
        }}
        )",
            (result.end - result.start) / 1ns,
            name,
            result.thread_id,
            ch::time_point_cast<ch::microseconds>(result.start)
                .time_since_epoch()
                .count());
        output_stream_.flush();
    }
    void TimerRecorder::write_profile_header() {
        std::unique_lock<std::mutex> lock(write_lock_);
        output_stream_ << R"(
        {
            "otherData": {},
            "traceEvents":[)";
        output_stream_.flush();
    }
    void TimerRecorder::write_profile_footer() {
        std::unique_lock<std::mutex> lock(write_lock_);
        output_stream_ << R"(
        {
            "otherData": {},
            "traceEvents":[)";
        output_stream_.flush();
    }
    TimerRecorder &TimerRecorder::get() {
        static TimerRecorder recorder;
        return recorder;
    }
    auto TimerWithRecorder::get_thread_id_hash(const std::thread::id &id) {
        return thread_hash_func(id);
    }
    void TimerWithRecorder::Stop() {
        auto end_time = hr_clock::now();
        auto duration = ((end_time - start_time_) / 1ns);
        auto thread_id = this->get_thread_id_hash(std::this_thread::get_id());
        TimerRecorder::get().dump_profile(
            {timer_name_, start_time_, end_time, thread_id});

#if TIMER_VERBOSE
        std::cout << (fmt::format(
            R"(
thread id: {} 
dur: {}ms
name: {})",
            thread_id,
            duration,
            timer_name_))
                  << std::endl;

#endif
    }

    TimerWithRecorder::~TimerWithRecorder() {
        Stop();
    }
    TimerWithRecorder::TimerWithRecorder(const char *timer_name)
        : timer_name_(timer_name) {
        this->start_time_ = hr_clock::now();
    }
}// namespace toy