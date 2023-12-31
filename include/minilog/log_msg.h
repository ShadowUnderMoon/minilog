#pragma once

#include <string>
#include <chrono>
#include <source_location>

#include "minilog/common.h"

namespace minilog {
struct log_msg {
    log_msg() = default;
    
    log_msg(const std::string &name, level::level_enum lvl, std::string_view msg, std::source_location loc) : logger_name(name), level(lvl), payload(msg), location(loc) {}

    std::string_view logger_name;
    level::level_enum level{level::off};
    std::string_view payload;
    std::chrono::zoned_time<std::chrono::milliseconds> time{std::chrono::current_zone(), std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())};
    std::source_location location;
};
}
