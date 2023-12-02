#pragma once

#include <memory>
#include <atomic>
namespace minilog {

template <typename T>
concept convertible_to_string_view = std::convertible_to<T, std::string_view>;

namespace sinks {
class sink;
}
using level_t = std::atomic<int>;
using sink_ptr = std::shared_ptr<sinks::sink>;
namespace level {
enum level_enum : int {
    trace,
    debug,
    info,
    warn,
    err,
    critical,
    off,
    n_levels
};
} // end namespace level
}
