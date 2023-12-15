#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/async.h>
#include <spdlog/cfg/env.h>

#include <minilog/sinks/basic_file_sink.h>

#include <iostream>

// multi/single threaded loggers
// console logging (colors supported)
// log file
// log filtering - log levels can be modified at runtime as well as compile time
// support for loading log levels from argv or environment var


void stdout_example() {
    // create a color multi-threaded logger
    auto console = spdlog::stdout_color_mt("console");
    auto err_logger = spdlog::stderr_color_mt("stderr");
    spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");
}

void basic_logfile_example() {
    try
    {
        auto logger = spdlog::basic_logger_mt("basic_logger", "spdlog.txt");
        logger->error("this is an error message");
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }
}

void minilog_basic_logfile() {
    auto logger = minilog::basic_logger_mt("basic_logger", "basic-log.txt");
    logger->error("this is an error message");
    logger->info(30);
    logger->set_level(minilog::level::debug);
    logger->info("{}: {}", "hello", 30);
    logger->debug("hello");
    logger->trace("shold not be printed");
}

// create a logger with 2 targets, with different log levels and formats
// The console will show only warning or errors, while the file will log all
void multi_sink_example() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);

    spdlog::logger logger("multi_sink", {console_sink, file_sink});
    logger.set_level(spdlog::level::debug);
    logger.warn("this should appear in both console and file");
    logger.info("this message should not appear in the console, only in the file");
}

// create a logger with a lambda function callback, the callback will be called
// each time something is logged to the logger
void callback_example()
{
    auto callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg& msg) {

    });
    callback_sink->set_level(spdlog::level::err);
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    spdlog::logger logger("custom_callback_logger", {console_sink, callback_sink});

    logger.info("some info log");
    logger.error("critical issue");
}

void async_example() {
    // default thread pool settings can be modified before creating the async logger
    // spdlog::init_thread_pool(8192, 1); // queue with 8k items and 1 backing thread
    auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger", "logs/async_log.txt");
    // alternatively:
    // auto sync_file = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>("async_file_logger", "logs/async_log.txt");
}


void multi_sink_example2()
{
    spdlog::init_thread_pool(8192, 1);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
    auto rotating_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("mylog.txt");
    std::vector<spdlog::sink_ptr> sinks {stdout_sink, rotating_sink};
    auto logger = std::make_shared<spdlog::async_logger>("loggername", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    spdlog::register_logger(logger);
}

void replace_default_logger_example() {
    auto new_logger = spdlog::basic_logger_mt("new_default_logger", "logs/new-default-log.txt", true);
    spdlog::set_default_logger(new_logger);
    spdlog::info("new logger log message");
}

void registry_base() {

    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d}; hex: {0:x}; oct: {0:o}; bin: {0: b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "two", "supported");
    spdlog::info("{:<30}", "left aligned");

    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("This message should be displayed...");

    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    spdlog::debug("hello world");
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("some debug message");


    spdlog::cfg::load_env_levels();
    // or from the command line
    // ./example SPDLOG_LEVEL=info, mylogger=trace
}


int main(int argc, char *argv[]) {
    // stdout_example();
    basic_logfile_example();
    minilog_basic_logfile();
}