#pragma once

#include <minilog/logger.h>
#include <minilog/registry.h>
#include <minilog/thread_pool.h>

namespace minilog {

static const size_t default_async_q_size = 8192;



class async_logger final : public std::enable_shared_from_this<async_logger>,
                           public logger {
    friend class thread_pool;
public:
    template <typename It>
    async_logger(std::string logger_name, It begin, It end,
                 std::weak_ptr<thread_pool> tp,
                 async_overflow_policy overflow_policy = async_overflow_policy::block)
        : logger(std::move(logger_name), begin, end),
          thread_pool_(std::move(tp)),
          overflow_policy_(overflow_policy) {}

    async_logger(std::string logger_name,
                 std::initializer_list<std::shared_ptr<sinks::sink>> sinks_list,
                 std::weak_ptr<thread_pool> tp,
                 async_overflow_policy overflow_policy = async_overflow_policy::block)
        : async_logger(std::move(logger_name), sinks_list.begin(), sinks_list.end(),
                       std::move(tp), overflow_policy) {}
    async_logger(std::string logger_name,
                 std::shared_ptr<sinks::sink> single_sink,
                 std::weak_ptr<thread_pool> tp,
                 async_overflow_policy overflow_policy = async_overflow_policy::block)
        : async_logger(std::move(logger_name), {std::move(single_sink)}, std::move(tp), overflow_policy) {}           
    // std::shared_ptr<logger> clone(std::string new_name) override;
    ~async_logger() = default;
protected:
    void sink_it_(const log_msg& msg) override {
        if (auto pool_ptr = thread_pool_.lock()) {
            pool_ptr->post_log(shared_from_this(), msg, overflow_policy_);
        } else {
            throw std::runtime_error("async log: thread pool doesn't exist anymore");
        }

        if (should_flush_(msg)) {
            backend_flush_();
        }
    }
    // void flush_() override;
    void backend_sink_it_(const log_msg& incoming_log_msg) {
        for (auto& sink : sinks_) {
            if (sink->should_log(incoming_log_msg.level)) {
                sink->log(incoming_log_msg);
            }
        }
    }
    void backend_flush_() {
        for (auto& sink : sinks_) {
            sink->flush();
        }
    }

private:
    std::weak_ptr<thread_pool> thread_pool_;
    async_overflow_policy overflow_policy_;
};

template <async_overflow_policy OverflowPolicy = async_overflow_policy::block>
struct async_factory_impl {
    template <typename Sink, typename... SinkArgs>
    static std::shared_ptr<async_logger> create(std::string logger_name, SinkArgs&&... args) {
        auto& registry_inst = registry::get_instance();

        std::unique_lock<std::recursive_mutex> tp_lock(registry_inst.tp_mutex());
        auto tp = registry_inst.get_tp();
        if (tp == nullptr) {
            tp = std::make_shared<thread_pool>(default_async_q_size, 1U);
            registry_inst.set_tp(tp);
        }

        auto sink = std::make_shared<Sink>(std::forward<SinkArgs>(args)...);
        auto new_logger = std::make_shared<async_logger>(std::move(logger_name), std::move(sink), std::move(tp), OverflowPolicy);
        return new_logger;
    }
};

using async_factory = async_factory_impl<async_overflow_policy::block>;
using async_factory_nonblock = async_factory_impl<async_overflow_policy::overrun_oldest>;

template <typename Sink, typename... SinkArgs>
inline std::shared_ptr<logger> create_async(std::string logger_name, SinkArgs&&... sink_args) {
    return async_factory::create<Sink>(std::move(logger_name), std::forward<SinkArgs>(sink_args)...);
}

template <typename Sink, typename... SinkArgs>
inline std::shared_ptr<logger> create_async_nb(std::string logger_name, SinkArgs&&... sink_args) {
    return async_factory_nonblock::create<Sink>(std::move(logger_name), std::forward<SinkArgs>(sink_args)...);
}

inline void init_thread_pool(size_t q_size, size_t thread_count,
                             std::function<void()> on_thread_start,
                             std::function<void()> on_thread_stop) {
    auto tp = std::make_shared<thread_pool>(q_size, thread_count, on_thread_start, on_thread_stop);
    registry::get_instance().set_tp(std::move(tp));
}

inline void init_thread_pool(size_t q_size, size_t thread_count, std::function<void()> on_thread_start) {
    init_thread_pool(q_size, thread_count, on_thread_start, [] {});
}

inline void init_thread_pool(size_t q_size, size_t thread_count) {
    init_thread_pool(q_size, thread_count, [] {}, [] {});
}

inline std::shared_ptr<thread_pool> thread_poool() {
    return registry::get_instance().get_tp();
}
}