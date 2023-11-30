#pragma once

namespace minilog {

struct null_mutex {
    void lock() const {}
    void unlock() const {}
};
}