#pragma once
#include "../public/pch.hpp"
#include "fmt/core.h"
#include "fmt/format.h"
#include <algorithm>
#include <bits/chrono.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ios>
#include <memory>
#include <mutex>
#include <string_view>
#include <thread>

using namespace std::chrono_literals;
#ifndef TIMEIT
#define TIMEIT TimerWithRecorder _time(__PRETTY_FUNCTION__)
#endif

#ifndef TIMER_SESSION_BEGIN
#define TIMER_SESSION_BEGIN(X) TimerRecorder::get().begin_session(X)
#endif

#ifndef TIMER_VERBOSE
#define TIMER_VERBOSE 1
#endif
#ifndef TIMER_SESSION_END
#define TIMER_SESSION_END TimerRecorder::get().end_session()
#endif
struct profile_result_t {
    using hr_time_point = std::chrono::high_resolution_clock::time_point;
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
    std::ofstream output_stream_;
    std::int64_t profile_count_;
    std::mutex write_lock_;

  public:
    TimerRecorder() : current_session_(nullptr), profile_count_(0) {
    }
    void begin_session(
        const std::string &session_name,
        const std::string &filepath = "result.json") {
        output_stream_.open(filepath, std::ios::out);
        current_session_
            = std::make_unique<timer_session_t>(timer_session_t{session_name});
        this->write_profile_header();
    }
    void end_session() {
        this->write_profile_footer();
        output_stream_.close();
        current_session_.reset();
        profile_count_ = 0;
    }
    void dump_profile(const profile_result_t &result) {
        if (profile_count_ > 0) {
            output_stream_ << ",";
        }
        profile_count_++;
        std::string name = result.name;
        std::replace(name.begin(), name.end(), '"', '\'');
        std::unique_lock<std::mutex> lock(write_lock_);
        // output_stream_ << "{";
        // output_stream_ << R"("cat":"function",)";
        // output_stream_ << "\"dur\":" << (result.end - result.start) / 1ms
        //                << ',';
        // output_stream_ << R"("name":")" << name << "\",";
        // output_stream_ << R"("ph":"X",)";
        // output_stream_ << "\"pid\":0,";
        // output_stream_ << "\"tid\":" << result.thread_id << ",";
        // output_stream_
        //     << "\"ts\":"
        //     << std::chrono::time_point_cast<std::chrono::milliseconds>(
        //            result.start)
        //            .time_since_epoch()
        //            .count();
        // output_stream_ << "}";
        output_stream_ << fmt::format(
            R"(
            {{
                "cat": "function",
                "dur": {},
                "name": "{}",
                "ph": "X",
                "pid": 0,
                "tid": {},
                "ts": {}
            }}
        )",
            (result.end - result.start) / 1ms,
            name,
            result.thread_id,
            std::chrono::time_point_cast<std::chrono::microseconds>(
                result.start)
                .time_since_epoch()
                .count());
        output_stream_.flush();
    }
    void write_profile_header() {
        std::unique_lock<std::mutex> lock(write_lock_);
        output_stream_ << R"({"otherData": {},"traceEvents":[)";
        output_stream_.flush();
    }
    void write_profile_footer() {
        std::unique_lock<std::mutex> lock(write_lock_);
        output_stream_ << "]}";
        output_stream_.flush();
    }
    static auto &get() {
        static TimerRecorder recorder;
        return recorder;
    }
};
class TimerWithRecorder {
    using hr_clock = std::chrono::high_resolution_clock;

  private:
    auto get_thread_id_hash(const std::thread::id &id) {
        return thread_hash_func(id);
    }

  public:
    explicit TimerWithRecorder(const std::string_view &timer_name)
        : timer_name_(timer_name) {
        this->start_time_ = hr_clock::now();
    }
    void Stop() {
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
    ~TimerWithRecorder() {
        Stop();
    }

  private:
    std::hash<std::thread::id> thread_hash_func;
    hr_clock::time_point start_time_;
    std::string timer_name_;
};