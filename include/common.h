#pragma once

#include <atomic>
namespace minilog {
using level_t = std::atomic<int>;

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
