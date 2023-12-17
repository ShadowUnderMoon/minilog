#pragma once

#include <functional>

#include <minilog/synchronous_factory.h>
#include <minilog/null_mutex.h>
#include <minilog/log_msg.h>
#include <minilog/sinks/base_sink.h>

namespace minilog {

using custom_log_callback = std::function<void(const log_msg &msg)>;

namespace sinks {
template <typename Mutex>
class callback_sink final : public base_sink<Mutex> {
public:
    explicit callback_sink(const custom_log_callback &callback)
        : callback_{callback} {}
protected:
    void sink_it_(const log_msg &msg) override { callback_(msg); };
    void flush_() override {};
private:
    custom_log_callback callback_;
};

using callback_sink_mt = callback_sink<std::mutex>;
using callback_sink_st = callback_sink<null_mutex>;
} // namespace sinks

template <typename Factory = synchronous_factory>
inline std::shared_ptr<logger> callback_logger_mt(const std::string &logger_name,
                                                  const custom_log_callback &callback) {
        return Factory::template create<sinks::callback_sink_mt>(logger_name, callback);
}

template <typename Factory = synchronous_factory>
inline std::shared_ptr<logger> callback_logger_st(const std::string &logger_name,
                                                  const custom_log_callback &callback) {
        return Factory::template create<sinks::callback_sink_mt>(logger_name, callback);
}
} // namespace minilog