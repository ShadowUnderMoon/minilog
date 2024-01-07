#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/async.h>
#include <spdlog/cfg/env.h>

#include <minilog/minilog.h>
#include <minilog/sinks/basic_file_sink.h>
#include <minilog/sinks/stdout_color_sinks.h>
#include <minilog/sinks/callback_sink.h>
#include <minilog/cfg.h>
#include <minilog/sinks/db_sink.h>
#include <minilog/async_logger.h>
#include <iostream>

// multi/single threaded loggers
// console logging (colors supported)
// log file
// log filtering - log levels can be modified at runtime as well as compile time
// support for loading log levels from argv or environment var


void stdout_example() {
    // create a color multi-threaded logger
    auto console = spdlog::stdout_color_mt("console");
    console->set_level(spdlog::level::debug);
    console->debug("this is a debug message to terminal");
    console->error("this is an error message to terminal");
    auto err_logger = spdlog::stderr_color_mt("stderr");
    spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");
}

void minilog_stdout_example() {
    auto console = minilog::stdout_color_mt("minilog_console");
    console->set_level(minilog::level::debug);
    console->debug("this is a debug message to terminal");
    console->error("this is an error message to terminal");
    console->info("this is an info message to terminal");
    auto logger_from_registry = minilog::get("minilog_console");
    if (logger_from_registry) {
        logger_from_registry->info("a message from retrieved logger from registry");
    }
    auto err_logger = minilog::stderr_color_mt("minilog_stderr");
    err_logger->info("hello from stderr");
    auto console_nocolor = minilog::stdout_color_mt("minilog_console_nocolor", minilog::color_mode::never);
    console_nocolor->error("an message without color");
}

void basic_logfile_example() {
    try
    {
        auto logger = spdlog::basic_logger_mt("basic_logger", "logs/spdlog.txt");
        logger->error("this is an error message");
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }
}

void minilog_basic_logfile() {
    auto logger = minilog::basic_logger_mt("basic_logger", "logs/minilog_basic-log.txt");
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

void minilog_multi_sink() {
    auto console_sink = std::make_shared<minilog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(minilog::level::warning);

    auto file_sink = std::make_shared<minilog::sinks::basic_file_sink_mt>("logs/minilog_multisink.txt");

    minilog::logger logger("multi_sink", {std::move(console_sink), std::move(file_sink)});
    logger.set_level(minilog::level::debug);
    logger.warn("this should appear in both console and file");
    logger.info("this message should not appear in the console, only in the file");    
}

// create a logger with a lambda function callback, the callback will be called
// each time something is logged to the logger
void callback_example()
{
    auto callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg& msg) {
        std::cout << "email me error message: " << msg.payload << std::endl;
    });
    callback_sink->set_level(spdlog::level::err);
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    spdlog::logger logger("custom_callback_logger", {console_sink, callback_sink});

    logger.info("some info log");
    logger.error("critical issue");
}

void minilog_callback_example()
{
    auto callback_sink = std::make_shared<minilog::sinks::callback_sink_mt>([](const minilog::log_msg& msg) {
        std::cout << "email me error message: " << msg.payload << std::endl;
    });
    callback_sink->set_level(minilog::level::error);
    auto console_sink = std::make_shared<minilog::sinks::stdout_color_sink_mt>();
    minilog::logger logger("custom_callback_logger", {console_sink, callback_sink});

    logger.info("some info log");
    logger.error("critical issue");
}

void minilog_db_sink()
{
    auto callback_sink = std::make_shared<minilog::sinks::callback_sink_mt>(minilog::sinks::sink_to_db);
    callback_sink->set_level(minilog::level::error);
    auto console_sink = std::make_shared<minilog::sinks::stdout_color_sink_mt>();
    minilog::logger logger("custom_callback_logger", {console_sink, callback_sink});

    logger.info("some info log");
    logger.error("critical 'issue");
    logger.error("use double quotes \" double quotes");
    logger.error("use single quotes ' single quotes");

}

void async_example() {
    // default thread pool settings can be modified before creating the async logger
    // spdlog::init_thread_pool(8192, 1); // queue with 8k items and 1 backing thread
    auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger", "logs/async_log.txt");
    // alternatively:
    // auto sync_file = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>("async_file_logger", "logs/async_log.txt");
    for (int i = 0; i < 101; ++i) {
        async_file->info("Async message #{}", i);
    }
}

void minilog_async_example() {
    auto async_file = minilog::basic_logger_mt<minilog::async_factory>("minilog_async_file_logger", "logs/minilog_async_log.txt");

    for (int i = 0; i < 101; ++i) {
        async_file->info("async message #{}", i);
    }
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

void minilog_replace_default_logger() {
    minilog::info("default logger");
    auto new_stdout_logger = minilog::stdout_color_mt("console");
    minilog::set_default_logger(std::move(new_stdout_logger));
    minilog::info("new stdout logger");
    auto new_logger = minilog::basic_logger_mt("new_default_logger", "logs/minilog_new-default-log.txt");
    minilog::set_default_logger(std::move(new_logger));
    minilog::info("new logger log message");
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

    spdlog::cfg::load_env_levels();
}

void minilog_registry_base() {

    minilog::info("Welcome to spdlog!");
    minilog::error("Some error message with arg: {}", 1);

    minilog::warn("Easy padding in numbers like {:08d}", 12);
    minilog::critical("Support for int: {0:d}; hex: {0:x}; oct: {0:o}; bin: {0: b}", 42);
    minilog::info("Support for floats {:03.2f}", 1.23456);
    minilog::info("Positional args are {1} {0}..", "two", "supported");
    minilog::info("{:<30}", "left aligned");

    minilog::set_level(minilog::level::debug);
    minilog::debug("This message should be displayed...");

    minilog::cfg::load_env_levels();
    minilog::trace("the message should be printed when env var MINILOG_LEVEL = trace");
}

int main(int argc, char *argv[]) {
    // stdout_example();
    // minilog_stdout_example();

    // basic_logfile_example();
    // minilog_basic_logfile();

    // registry_base();
    // minilog_registry_base();

    // replace_default_logger_example();
    // minilog_replace_default_logger();

    // multi_sink_example();
    // minilog_multi_sink();

    // callback_example();
    // minilog_callback_example();

    // minilog_db_sink();

    async_example();
    minilog_async_example();
}