#pragma once

#include <minilog/common.h>
#include <minilog/log_msg.h>
namespace minilog::sinks {

class sink {
public:
    virtual ~sink() = default;
    virtual void log(const log_msg &msg) = 0;
    virtual void flush() = 0;
    
    void set_level(level::level_enum log_level);
    level::level_enum level() const;
    bool should_log(level::level_enum msg_level) const;

protected:
    level_t level_{level::trace};
};

}