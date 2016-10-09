#include <theoria/util/CommandLine.h>
#include <theoria/except/except.h>

#include <cstring>
#include <boost/algorithm/string/predicate.hpp>

using namespace theoria ;
using namespace util ;

static CommandLine*  __CommandLine = nullptr ;

CommandLine::CommandLine(int argc, const char*argv[])
{
    __CommandLine = this ;

    if (argc < 1) 
        throw RUNTIME_ERROR("theoria requires at least a config file arg") ;

    //First the config file which is mandatory
    std::string config_file(argv[0]) ;
    FILE* f = fopen(config_file.c_str(), "r") ;
    if (!f)
        throw RUNTIME_ERROR("theoria could not open config %s", config_file.c_str()) ;
    _configFileName = config_file ;

    //Second optional settings which may be terminated with --
    int iSetting = 1 ;
    while(iSetting < argc && strcmp(argv[iSetting], "--") != 0) {
        const char * setting = argv[iSetting++] ;
        const char * value = "true" ;
        if (iSetting < argc && strncmp(argv[iSetting], "--", 2) != 0)
            value = argv[iSetting++] ;
        _settings[setting] = value ;
    }
   
    iSetting++ ; //eat potential trailing -- 
    if (iSetting >= argc)
        return ;

    //Variables
    const char * pEq = nullptr ;
    while(iSetting < argc && (pEq = strstr(argv[iSetting], "=")) != nullptr) {
        const char * p = argv[iSetting++] ;
        std::string name(p, pEq-p) ;
        std::string value(pEq+1) ;
        if ( name[0] != '_' || ('A' >= name[0] && name[0] <= 'Z') || ('a' >= name[0] && name[0] <= 'z') ) {
            _variables[name] = value ;
        }
        else {
            throw RUNTIME_ERROR("theoria variables must start with '_' or alpha: [%s] is illegal" , name.c_str()) ;
        }
    }

    if (iSetting < argc) {
        throw RUNTIME_ERROR("theoria found unexpected parameters starting with %s", argv[iSetting]) ;
    }
}

const CommandLine& CommandLine::instance() 
{
    //yes death will be swift if used before main initializes!
    return *__CommandLine ;
}

const char * CommandLine::variableAsPtr(const std::string& name) const noexcept 
{
    auto iter = findVar(name) ;
    if (iter != endVars())
        return iter->second.c_str() ;
    return nullptr ;
}

const std::string& CommandLine::variableAsStr(const std::string& name, const std::string& def) const 
{
    auto iter = findVar(name) ;
    if (iter != _variables.cend())
        return iter->second ;
    return def ;
}

int64_t CommandLine::variableAsInt(const std::string& name, int64_t def) const 
{
    auto iter = findVar(name) ;
    if (iter != _variables.cend())
    {
        char * err = nullptr;
        int64_t result = strtoll(iter->second.c_str(), &err, 10)  ;
        if (err && *err)
            throw RUNTIME_ERROR("variableAsInt(%s, %lld) %s is not an integer", name.c_str(), def, iter->second.c_str()) ;
        return result ;
    }
    return def ;
}

double CommandLine::variableAsDbl(const std::string& name, double def) const 
{
    auto iter = findVar(name) ;
    if (iter != _variables.cend())
    {
        char * err = nullptr;
        double result = strtod(iter->second.c_str(), &err)  ;
        if (err && *err)
            throw RUNTIME_ERROR("variableAsDbl(%s, %g) %s is not a double", name.c_str(), def, iter->second.c_str()) ;
        return result ;
    }
    return def ;
}

bool CommandLine::variableAsBool(const std::string& name, bool def) const 
{
    auto iter = findVar(name) ;
    if (iter != _variables.cend())
    {
        const char * value = iter->second.c_str() ;
        if ( boost::iequals(value, "false") || strcmp(value, "0") == 0 )
            return false ;
        if ( boost::iequals(value, "true") || strcmp(value, "1") == 0 )
            return true ;
        throw RUNTIME_ERROR("variableAsBool(%s, %s) %s is not a bool", name.c_str(), (def ? "true" : "false"), iter->second.c_str()) ;
    }
    return def ;
}

