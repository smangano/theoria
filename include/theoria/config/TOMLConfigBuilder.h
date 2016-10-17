#pragma once

#include <theoria/config/Builder.h>
#include <cpptoml.h>

namespace theoria { namespace config {

class TOMLConfigBuilder : private ConfigBuilder
{
public:

    TOMLConfigBuilder() {};

    Config* parse_file(const std::string& filename) ; 
    Config* parse(std::istream& stream) ; 

private:

    void _recursive_build(cpptoml::table& table) ;
    void _addAttr(const std::string& name, const std::string& value, const std::string& type) ;
    void _addAttrWithResolve(const std::string& name, const std::string& value, const std::string& type) ;
    std::pair<std::string, std::string> _getValueAndTypeAsString(const cpptoml::table& owner,const std::string& name) ;
    
} ;

}}
