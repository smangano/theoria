#include <theoria/util/strings.h>


namespace theoria { namespace util {

std::vector<std::string> string_split(const std::string& string, char sep) 
{
    std::istringstream iss(string) ;
    std::vector<std::string> result ;
    std::string item ;
    while (std::getline(iss,item, sep)) {
        result.push_back(item) ;
    }
    return result ;
}

}} 
