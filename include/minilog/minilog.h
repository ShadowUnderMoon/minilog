#pragma once

#include <minilog/common.h>
#include <minilog/registry.h>
#include <minilog/synchronous_factory.h>
#include <minilog/logger.h>

namespace minilog {

inline std::shared_ptr<logger> get(const std::string &name)
{
    return registry::get_instance().get(name);
}

inline std::shared_ptr<logger> get_default_logger()
{
    return get("");
}

inline void set_level(level::level_enum lvl) {
    get_default_logger()->set_level(lvl);
}

template <typename T>
void trace(const T &msg, std::source_location loc=std::source_location::current()) {
    get_default_logger()->trace(msg, loc);
}

template <typename T>
void debug(const T &msg, std::source_location loc=std::source_location::current()) {
    get_default_logger()->debug(msg, loc);
}

template <typename T>
void info(const T &msg, std::source_location loc=std::source_location::current()) {
    get_default_logger()->info(msg, loc);
}

template <typename T>
void warn(const T &msg, std::source_location loc=std::source_location::current()) {
    get_default_logger()->warn(msg, loc);
}

template <typename T>
void error(const T &msg, std::source_location loc=std::source_location::current()) {
    get_default_logger()->error(msg, loc);
}

template <typename T>
void critical(const T &msg, std::source_location loc=std::source_location::current()) {
    get_default_logger()->critical(msg, loc);
}

template <typename... Args>
void trace(FormatWithLocation fmt, Args &&...args) {
    get_default_logger()->trace(std::move(fmt), std::forward<Args>(args)...);
}

template <typename... Args>
void debug(FormatWithLocation fmt, Args &&...args) {
    get_default_logger()->debug(std::move(fmt), std::forward<Args>(args)...);
}

template <typename... Args>
void info(FormatWithLocation fmt, Args &&...args) {
    get_default_logger()->info(std::move(fmt), std::forward<Args>(args)...);
}

template <typename... Args>
void warn(FormatWithLocation fmt, Args &&...args) {
    get_default_logger()->warn(std::move(fmt), std::forward<Args>(args)...);
}

template <typename... Args>
void error(FormatWithLocation fmt, Args &&...args) {
    get_default_logger()->error(std::move(fmt), std::forward<Args>(args)...);
}

template <typename... Args>
void critical(FormatWithLocation fmt, Args &&...args) {
    get_default_logger()->critical(std::move(fmt), std::forward<Args>(args)...);
}


}