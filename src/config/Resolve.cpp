#include <theoria/config/Resolve.h>
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
//DisallowResolver 

Result DisallowResolver::lookup(const std::string& name) const
{
    RUNTIME_ERROR("Resolving variables disallowed while trying to resolve variable with name: [%s]", name.c_str()) ;
}

//////////////////////////
// DisableResolver

Result DisableResolver::lookup(const std::string& name) const
{
    return Result(this, name) ;
}


/////////////////////////
//TOMLResolver

TOMLResolver(const std::string& tomlFilePath) :
    _impl(new TOMLResolver::TOMLResolverImpl(tomlFilePath))
{
}

Result TOMLResolver::lookup(const std::string& name) const override
{
    _impl->lookup(name) ;
}

TOMLResolver::TOMLResolverImpl::(const std::string& tomlFilePath)
    :_tomlFilePath(tomlFilePath), _tomlTable(cpptoml::parse_file(tomlFilePath))
{
}

TOMLResolver::TOMLResolverImpl::lookup(const std::string& name)
{
    _tomlTable.get_qualified(name) ;
}
