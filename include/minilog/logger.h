#pragma once

#include <format>
#include <string>
#include <vector>
#include <concepts>

#include <minilog/common.h>
#include <minilog/log_msg.h>
#include <minilog/sinks/sink.h>

namespace minilog {


struct FormatWithLocation {
    std::string_view format;
    std::source_location location;

    template <typename T>
        requires std::convertible_to<T, std::string_view>
    FormatWithLocation(const T &fmt, std::source_location loc=std::source_location::current()):
        format(fmt), location(std::move(loc)) {}
};

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
          level_(other.level_.load(std::memory_order_relaxed)),
          flush_level_(other.flush_level_.load(std::memory_order_relaxed)) {}

    logger(logger&& other) noexcept
        : name_(std::move(other.name_)),
          sinks_(std::move(other.sinks_)),
          level_(other.level_.load(std::memory_order_relaxed)),
          flush_level_(other.flush_level_.load(std::memory_order_relaxed)) {}
    
    logger& operator=(logger other) noexcept {
        this->swap(other);
        return *this;
    }

    const std::string &name() const {
        return name_;
    }
    
    void swap(logger& other) noexcept {
        name_.swap(other.name_);
        sinks_.swap(other.sinks_);

        // swap level_
        auto other_level = other.level_.load();
        auto my_level = level_.exchange(other_level);
        other.level_.store(my_level);


        // swap flush level_
        other_level = other.flush_level_.load();
        my_level = flush_level_.exchange(other_level);
        other.flush_level_.store(my_level);
    }
    virtual ~logger() = default;

    void set_level(level::level_enum log_level) {
        level_.store(log_level);
    }

    level::level_enum level() const {
        return static_cast<level::level_enum>(level_.load(std::memory_order_relaxed));
    }

    level::level_enum flush_level() const {
        return static_cast<level::level_enum>(flush_level_.load(std::memory_order_relaxed));
    }

    bool should_flush(const log_msg &msg) {
        auto flush_level = flush_level_.load(std::memory_order_relaxed);
        return (msg.level >= flush_level) && (msg.level != level::off);
    }

    bool should_log(level::level_enum msg_level) const {
        return msg_level >= level_.load(std::memory_order_relaxed);
    }

    template <typename... Args>
    void log(level::level_enum lvl, FormatWithLocation format_with_location, Args &&...args) {
        bool log_enabled = should_log(lvl);
        std::string message = vformat(format_with_location.format, std::make_format_args(args...));
        log_msg log_message(name_, lvl, message, format_with_location.location);
        log_it_(log_message, log_enabled);
    }

    template <typename T>
        requires (!convertible_to_string_view<T>)
    void log(level::level_enum lvl, const T &msg, std::source_location loc=std::source_location::current()) {
        log(lvl, FormatWithLocation("{}", loc), msg);
    }

    void log(level::level_enum lvl, std::string_view msg, std::source_location loc=std::source_location::current()) {
        bool log_enabled = should_log(lvl);
        if (!log_enabled) return;
        log_msg log_message(name_, lvl, msg, loc);
        log_it_(log_message, log_enabled);
    }

    template <typename T>
    void trace(const T &msg, std::source_location loc=std::source_location::current()) {
        log(level::trace, msg, loc);
    }

    template <typename T>
    void debug(const T &msg, std::source_location loc=std::source_location::current()) {
        log(level::debug, msg, loc);
    }

    template <typename T>
    void info(const T &msg, std::source_location loc=std::source_location::current()) {
        log(level::info, msg, loc);
    }

    template <typename T>
    void warn(const T &msg, std::source_location loc=std::source_location::current()) {
        log(level::warning, msg, loc);
    }

    template <typename T>
    void error(const T &msg, std::source_location loc=std::source_location::current()) {
        log(level::error, msg, loc);
    }

    template <typename T>
    void critical(const T &msg, std::source_location loc=std::source_location::current()) {
        log(level::critical, msg, loc);
    }

    template <typename... Args>
    void trace(FormatWithLocation fmt, Args &&...args) {
        log(level::trace, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(FormatWithLocation fmt, Args &&...args) {
        log(level::debug, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(FormatWithLocation fmt, Args &&...args) {
        log(level::info, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(FormatWithLocation fmt, Args &&...args) {
        log(level::warning, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(FormatWithLocation fmt, Args &&...args) {
        log(level::error, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void critical(FormatWithLocation fmt, Args &&...args) {
        log(level::critical, fmt, std::forward<Args>(args)...);
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
    std::atomic<int> level_{level::info};
    std::atomic<int> flush_level_{level::off};
};

inline void swap(logger& a, logger& b) {
    a.swap(b);
}

}