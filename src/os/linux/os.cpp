#include "theoria/util/strings.h"
#include <sys/stat.h>

namespace theoria { namespace os {

std::vector<std::string> split_search_path(const std::string& path)
{
    return util::string_split(path, ':'); 
}

std::string join_path(
    const std::string& p1, 
    const std::string& p2, 
    const std::string& p3,
    const std::string& p4,
    const std::string& p5,
    const std::string& p6,
    const std::string& p7,
    const std::string& p8,
    const std::string& p9
    ) 
{
    std::string result(p1 + "/" + p2) ;
    for (auto p : {p3, p4, p5, p6, p7, p8, p9}) {
        if (p == "")
            break ;
        result += "/" + p ;
    }
    return result ;
}

bool exists(const std::string& filename) 
{
    struct stat buffer ;
    int status = stat(filename.c_str(), &buffer) ;
    if (status == 0) {
        if (buffer.st_mode & (S_IFREG|S_IFLNK)) 
            return true ;
    }
    return false ;
}

std::string bootstrap_path() 
{
    return "/usr/local/theoria:/usr/local/share/theoria" ;
}

}}
