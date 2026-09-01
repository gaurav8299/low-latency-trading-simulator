#include "common/logger.hpp"

#include <iomanip>
#include <sstream>

namespace hft {

void Logger::log(LogLevel level, std::string_view component, std::string_view message) {
    if (level < min_level_) {
        return;
    }

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()) % 1000000;

    std::ostringstream oss;
    std::tm tm_buf{};

#if defined(_MSC_VER) || defined(_WIN32)
    localtime_s(&tm_buf, &time_t_now);
#else
    localtime_r(&time_t_now, &tm_buf);
#endif

    oss << '['
        << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setfill('0') << std::setw(6) << us.count()
        << "] ["
        << to_string(level)
        << "] ["
        << component
        << "] "
        << message;

    std::string formatted = oss.str();

    std::lock_guard<std::mutex> lock(mutex_);

    if (console_output_) {
        if (level >= LogLevel::Warning) {
            std::cerr << formatted << '\n';
        } else {
            std::cout << formatted << '\n';
        }
    }

    if (file_.is_open()) {
        file_ << formatted << '\n';
        file_.flush();
    }
}

} // namespace hft
