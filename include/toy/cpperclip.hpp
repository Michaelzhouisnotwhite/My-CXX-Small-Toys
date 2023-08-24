#ifndef INCLUDE_TOY_CPPERCLIP
#define INCLUDE_TOY_CPPERCLIP
namespace toy::cpperclip {
    std::string wait_for_new_paste(int64_t timeout = -1);
    class CpperClipTimeoutError : public std::runtime_error {
    public:
        std::string msg_;
        explicit CpperClipTimeoutError(std::string msg = "");
    };

}// namespace toy::cpperclip

#endif /* INCLUDE_TOY_CPPERCLIP */
