#pragma once

#include <format>
#include <string>
#include <vector>

#include <minilog/common.h>
#include <minilog/log_msg.h>
#include <minilog/sinks/sink.h>

namespace minilog {

class logger {
public:
    explicit logger(std::string name)
        : name_(std::move(name)),
          sinks_() {}   

    template <typename It>
    logger(std::string name, It begin, It end)
        : name_(std::move(name)),
          sinks_(begin, end) {}
    
    logger(std::string name, sink_ptr single_sink)
        : logger(std::move(name), {std::move(single_sink)}) {}
    
    logger(std::string name, std::initializer_list<sink_ptr> sinks)
        : logger(std::move(name), sinks.begin(), sinks.end()) {}

    logger(const logger& other)
        : name_(other.name_),
          sinks_(other.sinks_),
          level_(other.level_),
          flush_level_(other.flush_level_) {}

    logger(logger&& other) noexcept
        : name_(std::move(other.name_)),
          sinks_(std::move(other.sinks_)),
          level_(other.level_),
          flush_level_(other.flush_level_) {}
    
    logger& operator=(logger other) noexcept {
        this->swap(other);
        return *this;
    }
    
    void swap(logger& other) noexcept {
        name_.swap(other.name_);
        sinks_.swap(other.sinks_);

        // swap level_
        std::swap(level_, other.level_);

        // swap flush level_
        std::swap(flush_level_, other.flush_level_);
    }
    virtual ~logger() = default;

    bool should_log(level::level_enum msg_level) const {
        std::unique_lock<std::mutex> lock(mtx);
        bool result = msg_level >= level_;
        return result;
    }
    template <typename... Args>
    void log(level::level_enum lvl, std::string_view fmt, Args &&...args) {
        log_(lvl, fmt, std::forward<Args>(args)...);
    }

    template <typename T>
        requires (!convertible_to_string_view<T>)
    void log(level::level_enum lvl, const T &msg) {
        log(lvl, "{}", msg);
    }

    void log(level::level_enum lvl, std::string_view msg) {
        bool log_enabled = should_log(lvl);
        if (!log_enabled) return;
        log_msg log_message(name_, lvl, msg);
        log_it_(log_message, log_enabled);
    }

    template <typename T>
    void trace(const T &msg) {
        log(level::trace, msg);
    }

    template <typename T>
    void debug(const T &msg) {
        log(level::debug, msg);
    }

    template <typename T>
    void info(const T &msg) {
        log(level::info, msg);
    }

    template <typename T>
    void warn(const T &msg) {
        log(level::warn, msg);
    }

    template <typename T>
    void error(const T &msg) {
        log(level::err, msg);
    }

    template <typename T>
    void critical(const T &msg) {
        log(level::critical, msg);
    }

    template <typename... Args>
    void log_(level::level_enum lvl, std::string_view fmt, Args &&...args) {
        bool log_enabled = true;
        std::string message = vformat(fmt, std::make_format_args(args...));
        log_msg log_message(name_, lvl, message);
        log_it_(log_message, log_enabled);
    }

    void log_it_(const log_msg &log_message, bool log_enabled)
    {
        if (log_enabled) {
            sink_it_(log_message);
        }
    }

    void sink_it_(const log_msg &msg) {
        for (auto &sink : sinks_) {
            if (sink->should_log(msg.level)) {
                sink->log(msg);
            }
        }
    }
protected:
    std::string name_;
    std::vector<sink_ptr> sinks_;
    level::level_enum level_{level::info};
    level::level_enum flush_level_{level::off};
    mutable std::mutex mtx;
};

inline void swap(logger& a, logger& b) {
    a.swap(b);
}

}