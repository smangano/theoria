#include <theoria/config/Resolve.h>
#include <theoria/except/except.h>
#include <theoria/util/CommandLine.h>

#include <cpptoml.h>
#include <cstdlib>

using namespace theoria ;
using namespace config ;

using Result = ConfigVariableResolver::Result ;

ConfigVariableResolver::~ConfigVariableResolver() 
{
    //delete _next ;
}

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
        Result result = resolver->lookup(name) ;
        if (result.first != nullptr)
            return result ; 
        resolver = resolver->_next ;
    }
    return Result(nullptr, "") ;
}

Result ConfigVariableResolver::resolveLast(const std::string& name) const 
{
    Result result(nullptr, "") ;
    const ConfigVariableResolver* resolver = this ;
    while(resolver) {
        Result temp = resolver->lookup(name) ;
        if (temp.first != nullptr)
            result = temp ;
        resolver = resolver->_next ;
    }
    return result ;
}

void ConfigVariableResolver::append(ConfigVariableResolver* resolver)
{
    if (! resolver)
        throw RUNTIME_ERROR("Can't append resolver to chain at [%s]: nullptr was provided in append!", name().c_str()) ;
    if (resolver->_next)
        throw RUNTIME_ERROR("Can't append resolver [%s] to chain at [%s]: appending resolver already part of a chain is not supported", resolver->name().c_str(), name().c_str()) ;

    ConfigVariableResolver * p = this ;
    ConfigVariableResolver * prev = nullptr ;
    while(p) {
        if (p == resolver)
            throw RUNTIME_ERROR("Can't append resolver [%s] to resolver chain: Already exists!", resolver->name().c_str()) ;
        prev = p ;
        p = p->_next ;
    }
    prev->_next = resolver ;
}

//////////////////////////
//EnvVarResolver

Result EnvVarResolver::lookup(const std::string& name) const  
{
    const char * value = std::getenv(name.c_str()) ;
    return value != nullptr ?  Result(this, value) : Result(nullptr, "") ;
    
}

std::string EnvVarResolver::name() const 
{
    return "EnvVarResolver" ;
}

//////////////////////////
//CmdLineResolver

Result CmdLineResolver::lookup(const std::string& name) const
{
    const char * value = util::CommandLine::instance().variableAsPtr(name) ;
    return value != nullptr ?  Result(this, value) : Result(nullptr, "") ;
}

std::string CmdLineResolver::name() const 
{
    return "CmdLineResolver" ;
}

//////////////////////////
//DisallowResolver 

Result DisallowResolver::lookup(const std::string& name) const
{
    throw RUNTIME_ERROR("Resolving variables disallowed while trying to resolve variable with name: [%s]", name.c_str()) ;
}

std::string DisallowResolver::name() const 
{
    return "DisallowResolver" ;
}

//////////////////////////
// DisableResolver

Result DisableResolver::lookup(const std::string& name) const
{
    return Result(this, name) ;
}


std::string DisableResolver::name() const 
{
    return "DisableResolver" ;
}



/**
 * TOMLResolver implementation
 */

class TOMLResolver::TOMLResolverImpl :  public ConfigVariableResolver
{
public:
    TOMLResolverImpl(const std::string& tomlFilePath) ;
    TOMLResolverImpl(std::istream& is) ;
    Result lookup(const std::string& name) const override ;
    std::string name() const override ;

private:

    std::string _tomlFilePath ;
    std::shared_ptr<cpptoml::table> _tomlTable ;
} ;


TOMLResolver::TOMLResolver(const std::string& tomlFilePath) :
    _impl(new TOMLResolver::TOMLResolverImpl(tomlFilePath))
{
}

TOMLResolver::TOMLResolver(std::istream& iss) :
    _impl(new TOMLResolver::TOMLResolverImpl(iss))
{
}

TOMLResolver::~TOMLResolver() {} 

Result TOMLResolver::lookup(const std::string& name) const 
{
    return _impl->lookup(name) ;
}

std::string TOMLResolver::name() const 
{
    return _impl->name() ;
}

TOMLResolver::TOMLResolverImpl::TOMLResolverImpl(const std::string& tomlFilePath)
    :_tomlFilePath(tomlFilePath), _tomlTable()
{
    try {
        _tomlTable = cpptoml::parse_file(tomlFilePath);
    }
    catch(const cpptoml::parse_exception& parseEx) {
        throw RUNTIME_ERROR("Could not parse TOML: %s", parseEx.what()) ;
    }
}

TOMLResolver::TOMLResolverImpl::TOMLResolverImpl(std::istream& is)
    :_tomlFilePath("<istream>"), _tomlTable()
{
    try {
        cpptoml::parser parser_(is) ;
        _tomlTable = parser_.parse() ;
    } 
    catch(const cpptoml::parse_exception& parseEx) {
        throw RUNTIME_ERROR("Could not parse TOML: %s", parseEx.what()) ;
    }
}


Result TOMLResolver::TOMLResolverImpl::lookup(const std::string& name) const
{
    
    auto base = _tomlTable->get_qualified(name) ;
    if (base->is_value()) {
        std::ostringstream oss ;
        auto strVal = base->as<std::string>() ;
        if (strVal) {
            std::string v = strVal->get() ;
            return Result(this, v) ;         
        }

        auto intVal = base->as<int64_t>() ;
        if (intVal) {
            oss << intVal->get() ;         
            return Result(this, oss.str()) ;
        }

        auto dblVal = base->as<double>() ;
        if (dblVal) {
            oss << dblVal->get() ;         
            return Result(this, oss.str()) ;
        }

        auto boolVal = base->as<bool>() ;
        if (boolVal) {
            oss << (boolVal->get() ? "true" : "false") ;         
            return Result(this, oss.str()) ;
        }

        auto dateVal = base->as<cpptoml::datetime>() ;
        if (dateVal) {
            auto dt = dateVal->get() ;
            oss << std::setfill('0') ;
            oss << std::setw(4) << dt.year << "-" << std::setw(2) << dt.month << "-" << std::setw(2) << dt.day ;
            if (dt.hour || dt.minute || dt.second || dt.microsecond)
            {
                oss << "T" << std::setw(2) << dt.hour << ":" << std::setw(2)  << dt.minute << ":" << std::setw(2) 
                    << dt.second ;
                if (dt.microsecond) 
                    oss << "." << std::setw(6) << dt.microsecond ;
                if (dt.hour_offset || dt.minute_offset) {
                    if (dt.hour_offset < 0) {
                        oss << "-" ;
                        dt.hour_offset *= -1 ;
                    }
                    oss << std::setw(2) << dt.hour_offset << ":" << std::setw(2) << dt.minute_offset ;
                }
            }
            return Result(this, oss.str()) ;
        }
        throw RUNTIME_ERROR("TOMLResolver unknown value type found!") ;
    }
    std::string found = base->is_table() ? "table" :  
        base->is_table_array() ? "table_array" : 
            base->is_array() ? "array" : "UNKNOWN" ;
    throw RUNTIME_ERROR("TOMLResolver can only resolve primitive values not [%s]", found.c_str()) ; 
}

std::string TOMLResolver::TOMLResolverImpl::name() const 
{
    return "TOMLResolver" ;
}

