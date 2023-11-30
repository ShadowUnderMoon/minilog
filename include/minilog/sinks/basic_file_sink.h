#pragma 
#include <minilog/sinks/base_sink.h>

namespace minilog::sinks {

template <typename Mutex>
class basic_file_sink final : public base_sink<Mutex> {
public:
    explicit basic_file_sink(const std::string &filename, bool truncate = false) {

    }
private:
                 
};
}