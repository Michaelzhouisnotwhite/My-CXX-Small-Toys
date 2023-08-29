#include "toy/cpperclip.hpp"
#include "private/pch.hpp"
#include <clip.h>
#ifndef INTERVAL_TIME
#define INTERVAL_TIME 300
#endif


namespace toy::cpperclip {
    CpperClipTimeoutError::CpperClipTimeoutError(std::string msg)
        : std::runtime_error(msg), msg_(std::move(msg)) {
    }
    
    std::string wait_for_new_paste(const int64_t timeout) {
        using hr_clock = std::chrono::high_resolution_clock;
        using hr_tp
            = std::chrono::time_point<std::chrono::high_resolution_clock>;
        std::optional<hr_tp> start_time;
        if (timeout != -1) {
            start_time = hr_clock::now();
        }
        static std::string last_paste;
        static bool first_run = true;
        if (first_run) {
            if (clip::has(clip::text_format())) {
                clip::get_text(last_paste);
            }
            first_run = false;
        }
        while (true) {

            if (!clip::has(clip::text_format())) {
                continue;
            }
            std::string temp_value;
            clip::get_text(temp_value);
            if (last_paste != temp_value) {
                last_paste = temp_value;
                return temp_value;
            }
            std::this_thread::sleep_for(
                std::chrono::milliseconds(INTERVAL_TIME));
            if (!start_time.has_value()) {
                continue;
            }
            auto duration = (hr_clock::now() - start_time.value()) / 1ms;
            if (duration >= timeout) {
                throw CpperClipTimeoutError("CpperClipTimeoutError: timeout! "
                                            "(you may need to catch this)");
            }
        }
    }
}// namespace toy::cpperclip