#pragma once
#include <minilog/common.h>
#include <minilog/sinks/sink.h>
#include <minilog/log_msg.h>

namespace minilog::sinks {
template <typename Mutex>
class base_sink : public sink {
public:
    base_sink() = default;
    ~base_sink() override = default;

    base_sink(const base_sink &) = delete;
    base_sink(base_sink &&) = delete;

    base_sink &operator=(const base_sink &) = delete;
    base_sink &operator=(base_sink &&) = delete;

    void log(const log_msg &msg) final {
        std::lock_guard<Mutex> lock(mutex_);
        sink_it_(msg);
    }

    void flush() final {
        std::lock_guard<Mutex> lock(mutex_);
        flush_();
    }

protected:
    Mutex mutex_;

    virtual void sink_it_(const log_msg &msg) = 0;
    virtual void flush_() = 0;
};
}