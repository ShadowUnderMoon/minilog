#pragma 

#include <minilog/common.h>
#include <minilog/registry.h>
#include <minilog/synchronous_factory.h>
#include <minilog/logger.h>

namespace minilog {
inline std::shared_ptr<logger> get(const std::string &name)
{
    return registry::get_instance().get(name);
}
}