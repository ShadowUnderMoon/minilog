#pragma once

#include <fstream>

namespace minilog {

class file_helper {
public:
    file_helper() = default;
    file_helper(const std::string &filename) : file_(filename, std::ios::out), filename_(filename) {}
    file_helper(const file_helper &) = delete;
    file_helper &operator=(const file_helper &) = delete;

    const std::string &filename() const {
        return filename_;
    }

    void write(const std::string &msg) {
        file_ << msg;
    }
private:
    std::ofstream file_;
    std::string filename_;
};

}