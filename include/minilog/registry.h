#pragma once

#include <string>
#include <unordered_map>
#include <shared_mutex>

#include <minilog/logger.h>
#include <minilog/sinks/ansicolor_sink.h>
namespace minilog {
class registry {
public:
    static registry& get_instance()
    {
        static registry instance;
        return instance;
    }
    registry(const registry &) = delete;
    registry &operator=(const registry &) = delete;

    void register_logger(std::shared_ptr<logger> new_logger) {
        std::unique_lock lock(mutex_);
        auto logger_name = new_logger->name();
        if (loggers_.find(logger_name) != loggers_.end()) {
            throw std::runtime_error(std::format("register a registerd logger: {}", logger_name));
        }
        loggers_[logger_name] = std::move(new_logger);
    }

    void drop_logger(const std::string &logger_name) {
        std::unique_lock lock(mutex_);
        loggers_.erase(logger_name);
    }

    std::shared_ptr<logger> get(const std::string &logger_name) {
        std::shared_lock lock(mutex_);
        auto found = loggers_.find(logger_name);
        return found == loggers_.end() ? nullptr : found->second;
    }

    std::shared_ptr<logger> get_default_logger() const {
        std::shared_lock lock(mutex_);
        return default_logger_name_ ? loggers_.at(default_logger_name_.value()) : nullptr;
    }

    void set_default_logger(std::shared_ptr<logger> new_default_logger) {
        std::unique_lock lock(mutex_);
        if (new_default_logger) {
            if (default_logger_name_) {
                loggers_.erase(default_logger_name_.value());
                default_logger_name_ = std::nullopt;
            }
            loggers_[new_default_logger->name()] = new_default_logger;
            default_logger_name_ = new_default_logger->name();
        }
    }
private:
    registry() {
        default_logger_name_ = "";
        auto color_sink = std::make_shared<sinks::ansicolor_stdout_sink_mt>();
        auto default_logger = std::make_shared<logger>(default_logger_name_.value(), std::move(color_sink));
        loggers_[default_logger_name_.value()] = std::move(default_logger);
    }
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<logger>> loggers_;
    std::optional<std::string> default_logger_name_;
};
}