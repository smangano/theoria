#include <stdexcept>
#include <cstdarg>

#define RUNTIME_ERROR(...) std::runtime_error(::theoria::except::format_except(__FILE__,__LINE__,__VA_ARGS__)) 

namespace theoria { namespace except {

std::string format_except(const char *file, int line, const char * fmt, ...) ;


}} //theoria::except


