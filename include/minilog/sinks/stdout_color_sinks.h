#pragma once
#include <minilog/logger.h>
#include <minilog/registry.h>
#include <minilog/synchronous_factory.h>

namespace minilog {
template <typename Factory = minilog::synchronous_factory>
std::shared_ptr<logger> stdout_color_mt(const std::string& logger_name, color_mode mode = color_mode::automatic) {
    return Factory::template create<sinks::stdout_color_sink_mt>(logger_name, mode);
}

template <typename Factory = minilog::synchronous_factory>
std::shared_ptr<logger> stdout_color_st(const std::string& logger_name, color_mode mode = color_mode::automatic) {
    return Factory::template create<sinks::stdout_color_sink_st>(logger_name, mode);
}

template <typename Factory = minilog::synchronous_factory>
std::shared_ptr<logger> stderr_color_mt(const std::string& logger_name, color_mode mode = color_mode::automatic) {
    return Factory::template create<sinks::stderr_color_sink_mt>(logger_name, mode);
}

template <typename Factory = minilog::synchronous_factory>
std::shared_ptr<logger> stderr_color_st(const std::string& logger_name, color_mode mode = color_mode::automatic) {
    return Factory::template create<sinks::stderr_color_sink_st>(logger_name, mode);
}
}