#include <theoria/config/resolve.h>
#include <theoria/except/except.h>
#include <theoria/util/CommandLine.h>

#include <cstdlib>

using namespace theoria ;
using namespace config ;

using Result = ConfigVariableResolver::Result ;

std::string ConfigVariableResolver::resolve(const std::string& var) const 
{
    if (var.size() < 2 || var[0] != '$') 
        throw RUNTIME_ERROR("resolve expects variables with leading '$' - got [%s] instead", var.c_str()) ;

    Result r ;
    if (var.size() > 2 && var[1] == '$')
       r = resolveLast(var.substr(2)) ;
    else
       r = resolveFirst(var.substr(1)) ;
    return r.first != nullptr ? r.second : var ;
}

Result ConfigVariableResolver::resolveFirst(const std::string& name) const 
{
    const ConfigVariableResolver* resolver = this ;
    while(resolver) {
        Result result = lookup(name) ;
        if (result.first != nullptr)
            return result ; 
        resolver = _next ;
    }
    return Result(nullptr, "") ;
}

Result ConfigVariableResolver::resolveLast(const std::string& name) const 
{
    Result result(nullptr, "") ;
    const ConfigVariableResolver* resolver = this ;
    while(resolver) {
        Result temp = lookup(name) ;
        if (temp.first != nullptr)
            result = temp ;
        resolver = _next ;
    }
    return result ;
}

//////////////////////////
//EnvVarResolver

Result EnvVarResolver::lookup(const std::string& name) const  
{
    const char * value = std::getenv(name.c_str()) ;
    return value != nullptr ?  Result(this, value) : Result(nullptr, "") ;
    
}

//////////////////////////
//CmdLineResolver

Result CmdLineResolver::lookup(const std::string& name) const
{
    const char * value = util::CommandLine::instance().variableAsPtr(name) ;
    return value != nullptr ?  Result(this, value) : Result(nullptr, "") ;
}

//////////////////////////
//
