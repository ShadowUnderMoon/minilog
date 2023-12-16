#pragma once

#include <iostream>
#include <string>
#include <magic_enum.hpp>

#include <minilog/minilog.h>
#include <minilog/common.h>

namespace minilog::cfg {
inline void load_env_levels() {
    const char * env_cstr = std::getenv("MINILOG_LEVEL");
    if (env_cstr) {
        minilog::set_level(magic_enum::enum_cast<minilog::level::level_enum>(env_cstr).value()); 
    }
}
}