#pragma once

#include <mutex>

#include <minilog/common.h>
#include <minilog/log_msg.h>
namespace minilog::sinks {

class sink {
public:
    virtual ~sink() = default;
    virtual void log(const log_msg &msg) = 0;
    virtual void flush() = 0;
    
    void set_level(level::level_enum log_level) {
        level_.store(log_level);
    }

    level::level_enum level() const {
        return static_cast<level::level_enum>(level_.load(std::memory_order_relaxed));
    }

    bool should_log(level::level_enum msg_level) const {
        return msg_level >= level_.load(std::memory_order_relaxed);
    }

protected:
    std::atomic<int> level_{level::trace};
    std::mutex mtx;
};

}