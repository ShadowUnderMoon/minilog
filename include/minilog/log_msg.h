#pragma once

#include <string>

#include "minilog/common.h"

namespace minilog {
struct log_msg {
    log_msg() = default;
    
    log_msg(const std::string &name, level::level_enum lvl, std::string_view msg) : logger_name(name), level(lvl), payload(msg) {}

    std::string_view logger_name;
    level::level_enum level{level::off};
    std::string_view payload;

};
}
