#ifndef INCLUDE_TOY_CPPERCLIP
#define INCLUDE_TOY_CPPERCLIP
#include "clip.h"
#ifndef INTERVAL_TIME
#define INTERVAL_TIME 300
#endif
namespace toy::cpperclip {
    class CpperClip {
    public:
        std::string last_paste_{};
        std::thread wt_, ticktock_thread_;
        std::function<void(std::string &result)> callback_;
        template<typename Func>
        explicit CpperClip(Func callback) {
            using namespace std::placeholders;
            if (clip::has(clip::text_format())) {
                clip::get_text(last_paste_);
            }
            callback_ = std::bind(callback, _1);

            wt_ = std::thread(&CpperClip::working_thread, this);
            ticktock_thread_ = std::thread(&CpperClip::ticktock_clock, this);
        }
        CpperClip(const CpperClip &) = delete;
        CpperClip(CpperClip &&) = delete;
        CpperClip &operator=(const CpperClip &) = delete;
        CpperClip &operator=(CpperClip &&) = delete;
        bool stop_flag_ = false;
        std::condition_variable cv_{};
        std::condition_variable clock_cv_{};
        std::mutex lock_{};
        bool timeout_ready = true;
        ~CpperClip() {
            // this->stop();
            wt_.join();
            ticktock_thread_.join();
        }
        void working_thread() {
            while (true) {
                std::unique_lock<std::mutex> lk(lock_);
                cv_.wait(lk, [this]() {
                    return (stop_flag_ || timeout_ready);
                });
                if (stop_flag_) {
                    return;
                }
                timeout_ready = false;
                lk.unlock();
                if (!clip::has(clip::text_format())) {
                    cv_.notify_all();
                    continue;
                }
                std::string temp_value;
                clip::get_text(temp_value);
                if (temp_value != last_paste_) {
                    callback_(temp_value);
                    last_paste_ = temp_value;
                    // this->stop();
                    // return;
                }
                cv_.notify_all();
            }
        }
        void ticktock_clock() {
            while (true) {
                std::unique_lock<std::mutex> lk(lock_);
                cv_.wait(lk, [this]() {
                    return !timeout_ready || stop_flag_;
                });
                if (stop_flag_) {
                    return;
                }
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(INTERVAL_TIME));
                timeout_ready = true;
                cv_.notify_all();
                lk.unlock();
            }
        }
        void stop() {
            std::unique_lock<std::mutex> lk(lock_);
            stop_flag_ = true;
            lk.unlock();
            cv_.notify_all();
            // clock_cv_.notify_all();
        }
    };
    std::string wait_for_new_paste(int64_t timeout = -1);
    class CpperClipTimeoutError : public std::runtime_error {
    public:
        std::string msg_;
        explicit CpperClipTimeoutError(std::string msg = "");
    };

}// namespace toy::cpperclip

#endif /* INCLUDE_TOY_CPPERCLIP */
