
#include "minilog/synchronous_factory.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <magic_enum.hpp>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <format>

#include <minilog/sinks/sink.h>
#include <minilog/common.h>
#include <minilog/null_mutex.h>

namespace minilog::sinks {

inline bool in_terminal(FILE *file) {
    return isatty(fileno(file)) != 0;
}

inline bool is_color_terminal() {
    static const bool result = []() {
        std::string env_colorterm = std::getenv("COLORTERM");
        if (env_colorterm.empty()) {
            return true;
        }

        std::array<std::string, 16> terms = {
            {"ansi", "color", "console", "cygwin", "gnome", "konsole", "kterm", "linux", "msys",
             "putty", "rxvt", "screen", "vt100", "xterm", "alacritty", "vt102"}};

        std::string env_term = std::getenv("TERM");
        if (env_term.empty()) {
            return false;
        }

        return std::any_of(terms.begin(), terms.end(), [&](const std::string &term) {
            return env_term.find(term) != std::string::npos;
        });
    }();

    return result;
}

template <typename Mutex>
struct console_mutex {
    using mutex_t = Mutex;
    static mutex_t &mutex() {
        static mutex_t s_mutex;
        return s_mutex;
    }
};

template <typename ConsoleMutex>
class ansicolor_sink : public sink {
public:
    using mutex_t = typename ConsoleMutex::mutex_t;
    ansicolor_sink(FILE *target_file, color_mode mode)
        : target_file_(target_file),
          mutex_(ConsoleMutex::mutex())
    {
        set_color_mode(mode);
        colors_.at(level::trace) = white;
        colors_.at(level::debug) = cyan;
        colors_.at(level::info) = green;
        colors_.at(level::warning) = yellow_bold;
        colors_.at(level::error) = red_bold;
        colors_.at(level::critical) = bold_on_red;
        colors_.at(level::off) = reset;
    }

    ~ansicolor_sink() override = default;

    ansicolor_sink(const ansicolor_sink &other) = delete;
    ansicolor_sink(ansicolor_sink &&other) = delete;

    ansicolor_sink &operator=(const ansicolor_sink &other) = delete;
    ansicolor_sink &operator=(ansicolor_sink &&other) = delete;

    void set_color(level::level_enum color_level, std::string_view color)
    {
        std::unique_lock<mutex_t> lock(mutex_);
        colors_.at(static_cast<size_t>(color_level)) = color;
    }

    void set_color_mode(color_mode mode) {
        if (mode == color_mode::always) {
            should_do_colors_ = true;
        } else if (mode == color_mode::automatic) {
            should_do_colors_ = in_terminal(target_file_) && is_color_terminal();
        } else {
            should_do_colors_ = false;
        }
    }

    bool should_color() {
        return should_do_colors_;
    }

    void log(const log_msg &msg) override {
        std::lock_guard<mutex_t> lock(mutex_);
        std::string formatted = format(msg);
        fwrite(formatted.data(), sizeof(char), formatted.size(), target_file_);
        fflush(target_file_);
    }

    void flush() override {
        std::unique_lock<mutex_t> lock(mutex_);
        fflush(target_file_);
    }

    std::string format(const log_msg &msg) {
        std::filesystem::path absolute_path = msg.location.file_name();
        std::string format_str = "{}:{} [{}] [{}] [{}] {}\n";
        if (should_color()) {
            format_str = colors_.at(msg.level) + format_str + std::string(reset);
        }
        return std::vformat(format_str, std::make_format_args(std::string(absolute_path.filename()), msg.location.line(), msg.time, msg.logger_name, magic_enum::enum_name(msg.level), msg.payload));
    }
    // Formatting codes
    const std::string_view reset = "\033[m";
    const std::string_view bold = "\033[1m";
    const std::string_view dark = "\033[2m";
    const std::string_view underline = "\033[4m";
    const std::string_view blink = "\033[5m";
    const std::string_view reverse = "\033[7m";
    const std::string_view concealed = "\033[8m";
    const std::string_view clear_line = "\033[K";

    // Foreground colors
    const std::string_view black = "\033[30m";
    const std::string_view red = "\033[31m";
    const std::string_view green = "\033[32m";
    const std::string_view yellow = "\033[33m";
    const std::string_view blue = "\033[34m";
    const std::string_view magenta = "\033[35m";
    const std::string_view cyan = "\033[36m";
    const std::string_view white = "\033[37m";

    /// Background colors
    const std::string_view on_black = "\033[40m";
    const std::string_view on_red = "\033[41m";
    const std::string_view on_green = "\033[42m";
    const std::string_view on_yellow = "\033[43m";
    const std::string_view on_blue = "\033[44m";
    const std::string_view on_magenta = "\033[45m";
    const std::string_view on_cyan = "\033[46m";
    const std::string_view on_white = "\033[47m";

    /// Bold colors
    const std::string_view yellow_bold = "\033[33m\033[1m";
    const std::string_view red_bold = "\033[31m\033[1m";
    const std::string_view bold_on_red = "\033[1m\033[41m";

private:
    FILE *target_file_;
    mutex_t &mutex_;
    bool should_do_colors_;
    std::array<std::string, level::n_levels> colors_;
};

template <typename ConsoleMutex>
class ansicolor_stdout_sink : public ansicolor_sink<ConsoleMutex> {
public:
    explicit ansicolor_stdout_sink(color_mode mode = color_mode::automatic)
        : ansicolor_sink<ConsoleMutex>(stdout, mode) {}
};

template <typename ConsoleMutex>
class ansicolor_stderr_sink : public ansicolor_sink<ConsoleMutex> {
public:
    explicit ansicolor_stderr_sink(color_mode mode = color_mode::automatic)
        : ansicolor_sink<ConsoleMutex>(stderr, mode) {}
};

using ansicolor_stdout_sink_mt = ansicolor_stdout_sink<console_mutex<std::mutex>>;
using ansicolor_stdout_sink_st = ansicolor_stdout_sink<console_mutex<minilog::null_mutex>>;
using ansicolor_stderr_sink_mt = ansicolor_stderr_sink<console_mutex<std::mutex>>;
using ansicolor_stderr_sink_st = ansicolor_stderr_sink<console_mutex<minilog::null_mutex>>;

using stdout_color_sink_mt = ansicolor_stdout_sink_mt;
using stdout_color_sink_st = ansicolor_stdout_sink_st;
using stderr_color_sink_mt = ansicolor_stderr_sink_mt;
using stderr_color_sink_st = ansicolor_stderr_sink_st;
}

namespace minilog {
template <typename Factory = minilog::synchronous_factory>
std::shared_ptr<logger> stdout_color_mt(const std::string& logger_name, color_mode mode = color_mode::automatic) {
    return Factory::template create<sinks::stdout_color_sink_mt>(logger_name, mode);
}

template <typename Factory = minilog::synchronous_factory>
std::shared_ptr<logger> stdout_color_st(const std::string& logger_name, color_mode mode = color_mode::automatic) {
    return Factory::template create<sinks::stdout_color_sink_st>(logger_name, mode);
}

template <typename Factory = minilog::synchronous_factory>
std::shared_ptr<logger> stderr_color_mt(const std::string& logger_name, color_mode mode = color_mode::automatic) {
    return Factory::template create<sinks::stderr_color_sink_mt>(logger_name, mode);
}

template <typename Factory = minilog::synchronous_factory>
std::shared_ptr<logger> stderr_color_st(const std::string& logger_name, color_mode mode = color_mode::automatic) {
    return Factory::template create<sinks::stderr_color_sink_st>(logger_name, mode);
}
}