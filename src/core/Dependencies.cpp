#include <theoria/core/Dependencies.h>
#include <sstream>

using namespace theoria ;
using namespace core ;

std::string Dependencies::Dependent::str() const
{
    std::ostringstream oss ;
    if (optional)
        oss << "Optional(" ;
    else
        oss << "Dependent(" ;
    oss << type ;
    if (subtype != "")
        oss << ":" << subtype ;
    oss << ")" ;
    return oss.str() ;
}

std::ostream& operator <<(std::ostream& os, const theoria::core::Dependencies::Dependent& dep)
{
    os << dep.str() ;
    return os ;
}

