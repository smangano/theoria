#include <theoria/core/AppConfigResolver.h>
#include <theoria/core/Registry.h>
#include <theoria/config/Config.h>
#include <theoria/util/strings.h>

using namespace theoria ;
using namespace core ;

/////////////////////////
//ConfigResolver

using Result = config::ConfigVariableResolver::Result ;
using Config = config::Config ;

std::string AppConfigResolver::name() const
{
    return "AppConfigResolver" ;
}

Result AppConfigResolver::lookup(const std::string& name) const 
{
    
    const Config * config = &core::Registry::instance().appConfig() ; 
    auto names = util::string_split(name, '.') ;
    int i = 0 ;
    for (;i<(int)names.size()-1 && config; i++) {
        config = config->getChild(names[i]) ;
    }
    if (config) {
        auto val = config->getAttrAsStr(names[i], "\1\1");
        if (val != "\1\1") 
            return Result(this, val) ;
    }    
    return Result(nullptr, "") ; 
}


