#include <stdexcept>

const char * format_except(const char *file, int line, const char * fmt, ...) ;

#define RUNTIME_ERROR(...) std::runtime_error(format_except(__FILE__,__LINE__,__VA_ARGS__)) 
