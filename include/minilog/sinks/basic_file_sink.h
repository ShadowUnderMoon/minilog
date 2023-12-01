#pragma

#include <mutex>
#include <memory>

#include <minilog/sinks/base_sink.h>
#include <minilog/synchronous_factory.h>
#include <minilog/file_helper.h>
#include <minilog/null_mutex.h>

namespace minilog {
namespace sinks {

template <typename Mutex>
class basic_file_sink final : public base_sink<Mutex> {
public:
    explicit basic_file_sink(const std::string &filename, bool truncate = false) {

    }

    const std::string &filename() const {

    }

protected:
    void sink_it_(const log_msg &msg) override {

    }

    void flush_() override {

    }
private:
    file_helper file_helper_;       
};

using basic_file_sink_mt = basic_file_sink<std::mutex>;
using basic_file_sink_st = basic_file_sink<null_mutex>;
} // end of namespace sinks

template <typename Factory = minilog::synchronous_factory>
std::shared_ptr<logger> basic_logger_mt(const std::string &logger_name,
                                        const std::string &filename,
                                        bool truncate = false)
{
    return Factory::template create<sinks::basic_file_sink_mt>(logger_name, filename, truncate);
}

template <typename Factory = minilog::synchronous_factory>
std::shared_ptr<logger> basic_logger_st(const std::string &logger_name,
                                        const std::string &filename,
                                        bool truncate = false)
{
    return Factory::template create<sinks::basic_file_sink_st>(logger_name, filename, truncate);
}
}