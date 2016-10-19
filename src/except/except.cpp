#include <theoria/except/except.h>
#include <cstring>
#include <cstdio>

namespace theoria { namespace except {

int strcount(const char * input, char c)
{
    int count = 0 ;
    const char* p = strchr(input, c) ;
    while (p) {
        ++count ;
        p = strchr(p+1, c) ;
    }
    return count ;
}

std::string format_except(const char *file, int line, const char * fmt,  ...) 
{
    va_list args ;
    va_start(args, fmt) ;
    int nestimate = strlen(file) + 8 + strlen(fmt) + (strcount(fmt,'%') * 20) + 512  ;
    std::string result(nestimate+1, '\0') ;
    int used = snprintf(const_cast<char *>(result.data()), nestimate, "%s:%d ", file, line) ;
    vsprintf(const_cast<char *>(result.data())+used, fmt, args) ;
    va_end(args) ;
    return result ;
}

}} //
