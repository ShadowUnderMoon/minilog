#pragma once

#include <minilog/common.h>

namespace minilog {
class file_helper {
public:
    file_helper() = default;

    file_helper(const file_helper &) = delete;
    file_helper &operator=(const file_helper &) = delete;

    void open(const std::string &fname, bool truncate = false) {

    }

    void reopen(bool truncate) {

    }

    void flush() {

    }

    void sync() {

    }

    void close() {

    }

    void write(const std::string &buf) {

    }

    size_t size() const {

    }

    const std::string &filename() const {

    }

    static std::tuple<std::string, std::string> split_by_extension(const std::string &fname) {

    }

private:
    const int open_tries_ = 5;
    const unsigned int open_interval_ = 10;
    std::FILE *fd_{nullptr};
    std::string filename_;
};
}