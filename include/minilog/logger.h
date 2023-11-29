#pragma once

#include <string>
#include <vector>

#include <minilog/common.h>

namespace minilog {

class logger {
public:
    explicit logger(std::string name)
        : name_(std::move(name)),
          sinks_() {}   

    template <typename It>
    logger(std::string name, It begin, It end)
        : name_(std::move(name)),
          sinks_(begin, end) {}
    
    logger(std::string name, sink_ptr single_sink)
        : logger(std::move(name), {std::move(single_sink)}) {}
    
    logger(std::string name, std::initializer_list<sink_ptr> sinks)
        : logger(std::move(name), sinks.begin(), sinks.end()) {}

    logger(const logger& other)
        : name_(other.name_),
          sinks_(other.sinks_),
          level_(other.level_.load(std::memory_order_relaxed)),
          flush_level_(other.flush_level_.load(std::memory_order_relaxed)) {}

    logger(logger&& other) noexcept
        : name_(std::move(other.name_)),
          sinks_(std::move(other.sinks_)),
          level_(other.level_.load(std::memory_order_relaxed)),
          flush_level_(other.flush_level_.load(std::memory_order_relaxed)) {}
    
    logger& operator=(logger other) noexcept {
        this->swap(other);
        return *this;
    }
    
    void swap(logger& other) noexcept {
        name_.swap(other.name_);
        sinks_.swap(other.sinks_);

        // swap level_
        auto other_level = other.level_.load();
        auto my_level = level_.exchange(other_level);
        other.level_.store(my_level);

        // swap flush level_
        other_level = other.flush_level_.load();
        my_level = flush_level_.exchange(other_level);
        other.flush_level_.store(my_level);
    }
    virtual ~logger() = default;

protected:
    std::string name_;
    std::vector<sink_ptr> sinks_;
    level_t level_{level::info};
    level_t flush_level_{level::off};
};

inline void swap(logger& a, logger& b) {
    a.swap(b);
}

}