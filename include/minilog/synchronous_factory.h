#pragma once

#include <memory>
#include <string>

#include <minilog/logger.h>
#include <minilog/registry.h>
namespace minilog {

struct synchronous_factory {
    template <typename Sink, typename... SinkArgs>
    static std::shared_ptr<logger> create(std::string logger_name, SinkArgs &&...args) {
        auto sink = std::make_shared<Sink>(std::forward<SinkArgs>(args)...);
        auto new_logger = std::make_shared<logger>(std::move(logger_name), std::move(sink));
        registry::get_instance().register_logger(new_logger);
        return new_logger;
    }
};
}