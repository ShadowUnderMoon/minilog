#pragma once

#include "minilog/log_msg.h"
#include <cassert>
#include <minilog/mpmc_blocking_q.h>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <thread>
namespace minilog {

class async_logger;
enum class async_overflow_policy {
    block,
    overrun_oldest,
    discard_new
};

class log_msg_buffer : public log_msg {
    std::string buffer;
    void update_string_views() {
        logger_name = std::string_view{buffer.data(), logger_name.size()};
        payload = std::string_view{buffer.data() + logger_name.size(), payload.size()};
    }

public:
    log_msg_buffer() = default;
    explicit log_msg_buffer(const log_msg& orig_msg)
        : log_msg(orig_msg) {
        buffer.append(logger_name.begin(), logger_name.end());
        buffer.append(payload.begin(), payload.end());
        update_string_views();

    }
    log_msg_buffer(const log_msg_buffer& other)
        : log_msg(other) {
        buffer.append(logger_name.begin(), logger_name.end());
        buffer.append(payload.begin(), payload.end());
        update_string_views();

    }
    log_msg_buffer(log_msg_buffer&& other) noexcept
        : log_msg{other}, buffer{std::move(other.buffer)} {
        update_string_views();
    }
    log_msg_buffer& operator=(const log_msg_buffer& other) {
        if (this != &other) {
            log_msg::operator=(other);
            buffer.clear();
            buffer.append(other.buffer.data(), other.buffer.data() + other.buffer.size());
            update_string_views();
        }
        return *this;
    }
    log_msg_buffer& operator=(log_msg_buffer&& other) noexcept {
        if (this != &other) {
            log_msg::operator=(other);
            buffer = std::move(other.buffer);
            update_string_views();
        }
        return *this;
    }
};

enum class async_msg_type {log, flush, terminate};

struct async_msg : log_msg_buffer {
    async_msg_type msg_type{async_msg_type::log};
    std::shared_ptr<async_logger> worker_ptr;

    async_msg() = default;
    ~async_msg() = default;

    async_msg(const async_msg &) = delete;
    async_msg(async_msg &&) = default;
    async_msg& operator=(async_msg &&) = default;

    async_msg(std::shared_ptr<async_logger>&& worker, async_msg_type the_type, const log_msg& m)
        : log_msg_buffer{m}, msg_type{the_type}, worker_ptr{std::move(worker)} {}
    async_msg(std::shared_ptr<async_logger>&& worker, async_msg_type the_type)
        : log_msg_buffer{}, msg_type{the_type}, worker_ptr{std::move(worker)} {}
    explicit async_msg(async_msg_type the_type)
        : async_msg{nullptr, the_type} {}
};

class thread_pool {
public:
    using item_type = async_msg;
    using q_type = mpmc_blocking_queue<item_type>;

    thread_pool(size_t q_max_items,
                size_t threads_n,
                std::function<void()> on_thread_start,
                std::function<void()> on_thread_stop)
        : q_(q_max_items)
    {
        if (threads_n == 0 || threads_n > 1000) {
            throw std::runtime_error("invalid threads_n params (range is 1-1000)");
        }
        for (size_t i = 0; i < threads_n; i++) {
            threads_.emplace_back([this, on_thread_start, on_thread_stop] {
                on_thread_start();
                this->worker_loop_();
                on_thread_stop();
            });
        }
    }

    thread_pool(size_t q_max_items,
                size_t threads_n,
                std::function<void()> on_thread_start)
        : thread_pool(q_max_items, threads_n, on_thread_start, [] {}) {}
    
    thread_pool(size_t q_max_items, size_t threads_n)
        : thread_pool(q_max_items, threads_n, [] {}, [] {}) {}
    
    ~thread_pool() {
        for (size_t i = 0; i < threads_.size(); i++) {
            post_async_msg_(async_msg(async_msg_type::terminate), async_overflow_policy::block);
        }
    }

    thread_pool(const thread_pool &) = delete;
    thread_pool& operator=(thread_pool &&) = delete;

    void post_log(std::shared_ptr<async_logger>&& worker_ptr,
                  const log_msg& msg,
                  async_overflow_policy overflow_policy)
    {
        async_msg async_m(std::move(worker_ptr), async_msg_type::log, msg);
        post_async_msg_(std::move(async_m), overflow_policy);
    }

    void post_flush(std::shared_ptr<async_logger>&& worker_ptr,
                    async_overflow_policy overflow_policy)
    {
        post_async_msg_(async_msg(std::move(worker_ptr), async_msg_type::flush), overflow_policy);
    }

    size_t overrun_counter() {
        return q_.overrun_counter();
    }

    void reset_overrun_counter() {
        q_.reset_overrun_counter();
    }

    size_t discard_counter() {
        return q_.discard_counter();
    }

    void reset_discard_counter() {
        q_.reset_discard_counter();
    }

    size_t queue_size() {
        return q_.size();
    }

private:
    q_type q_;
    std::vector<std::jthread> threads_;

    void post_async_msg_(async_msg&& new_msg, async_overflow_policy overflow_policy) {
        if (overflow_policy == async_overflow_policy::block) {
            q_.enqueue(std::move(new_msg));
        } else if (overflow_policy == async_overflow_policy::overrun_oldest) {
            q_.enqueue_nowait(std::move(new_msg));
        } else {
            assert(overflow_policy == async_overflow_policy::discard_new);
            q_.enqueue_if_have_room(std::move(new_msg));
        }
    }

    void worker_loop_() {
        while (process_next_msg_()) {

        }
    }

    bool process_next_msg_();
};
}