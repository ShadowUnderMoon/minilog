#pragma once

#include <string>
#include <unordered_map>

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
        std::lock_guard<std::mutex> lock(mtx);
        auto logger_name = new_logger->name();
        if (loggers_.find(logger_name) != loggers_.end()) {
            throw std::runtime_error(std::format("register a registerd logger: {}", logger_name));
        }
        loggers_[logger_name] = std::move(new_logger);
    }

    void drop_logger(const std::string &logger_name) {
        std::lock_guard<std::mutex> lock(mtx);
        loggers_.erase(logger_name);
    }

    std::shared_ptr<logger> get(const std::string &logger_name) {
        std::lock_guard<std::mutex> lock(mtx);
        auto found = loggers_.find(logger_name);
        return found == loggers_.end() ? nullptr : found->second;
    }

private:
    registry() {
        const std::string default_logger_name = "";
        auto color_sink = std::make_shared<sinks::ansicolor_stdout_sink_mt>();
        auto default_logger = std::make_shared<logger>(default_logger_name, std::move(color_sink));
        loggers_[default_logger_name] = std::move(default_logger);
    }
    std::mutex mtx;
    std::unordered_map<std::string, std::shared_ptr<logger>> loggers_;
    
};
}