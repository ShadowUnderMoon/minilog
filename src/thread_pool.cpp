#include <minilog/thread_pool.h>
#include <minilog/async_logger.h>

bool minilog::thread_pool::process_next_msg_() {
    async_msg incoming_async_msg;
    q_.dequeue(incoming_async_msg);

    if (incoming_async_msg.msg_type == async_msg_type::log) {
        incoming_async_msg.worker_ptr->backend_sink_it_(incoming_async_msg);
        return true;
    } else if (incoming_async_msg.msg_type == async_msg_type::flush) {
        incoming_async_msg.worker_ptr->backend_flush_();
        return true;
    } else if (incoming_async_msg.msg_type == async_msg_type::terminate) {
        return false;
    } else {
        assert(false);
    }
    return true;
}


