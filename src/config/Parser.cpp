#include <theoria/config/Parser.h>
#include <theoria/except/except.h>

#include <fstream>

using namespace theoria ;
using namespace config ;

Config* ConfigParser::parse_file(const std::string& filename) 
{
    std::ifstream ifs(filename.c_str()) ;
    if (!ifs.good()) 
        throw RUNTIME_ERROR("Could not open config file [%s]", filename.c_str()) ;
    return parse(ifs) ;
}
