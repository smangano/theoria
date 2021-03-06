#include <theoria/util/CommandLine.h>
#include <theoria/util/conversions.h>
#include <theoria/except/except.h>

#include <cstring>

using namespace theoria ;
using namespace util ;

static CommandLine*  __CommandLine = nullptr ;

CommandLine::CommandLine(int argc, const char*argv[], bool allowMissingConfig)
{
    //The first one constructed is the global instance
    if (!__CommandLine) 
        __CommandLine = this ;

    if (argc < 1) 
        throw RUNTIME_ERROR("theoria requires at least a config file arg") ;

    //First optional settings which may be terminated with --
    int iSetting = 0 ;
    while(iSetting < argc && strcmp(argv[iSetting], "--") != 0 && argv[iSetting] == strstr(argv[iSetting], "--")) {
        const char * setting = argv[iSetting++]+2 ;
        const char * value = "true" ;
        if (iSetting < argc && strncmp(argv[iSetting], "--", 2) != 0)
            value = argv[iSetting++] ;
        _settings[setting] = value ;
    }
    if (iSetting < argc && strcmp(argv[iSetting], "--") == 0)
        ++iSetting ;

    if (iSetting >= argc) {
        if (hasSetting("help") || allowMissingConfig)
            return ;
        throw RUNTIME_ERROR("Missing config file");
    }

    //Next the config file which is mandatory
    std::string config_file(argv[iSetting++]) ;
    if (!allowMissingConfig) {
        FILE* f = fopen(config_file.c_str(), "r") ;
        if (!f)
            throw RUNTIME_ERROR("theoria could not open config %s", config_file.c_str()) ;
        fclose(f) ;
    }
    _configFileName = config_file ;

    //Next optional user settings which may be terminated with --
    while(iSetting < argc && strcmp(argv[iSetting], "--") != 0 && argv[iSetting] == strstr(argv[iSetting], "--")) {
        const char * setting = argv[iSetting++]+2 ;
        const char * value = "true" ;
        if (iSetting < argc && strncmp(argv[iSetting], "--", 2) != 0)
            value = argv[iSetting++] ;
        if (hasSetting(setting)) 
            throw RUNTIME_ERROR("User setting duplicates existing setting: %s", setting) ;
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
        if ( name[0] == '_' || ('A' <= name[0] && name[0] <= 'Z') || ('a' <= name[0] && name[0] <= 'z') ) {
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
    return convertToInt(findVar(name) , endVars(), "variable", name, def) ;
}

double CommandLine::variableAsDbl(const std::string& name, double def) const 
{
    return convertToDbl(findVar(name) , endVars(), "variable", name, def) ;
}

bool CommandLine::variableAsBool(const std::string& name, bool def) const 
{
    return convertToBool(findVar(name) , endVars(), "variable", name, def) ;
}

void CommandLine::reset()
{
    __CommandLine = nullptr ;
}

const char * CommandLine::settingAsPtr(const std::string& name) const noexcept 
{
    auto iter = findSetting(name) ;
    if (iter != endSettings())
        return iter->second.c_str() ;
    return nullptr ;
}

const std::string& CommandLine::settingAsStr(const std::string& name, const std::string& def) const 
{
    auto iter = findSetting(name) ;
    if (iter != endSettings())
        return iter->second ;
    return def ;
}

int64_t CommandLine::settingAsInt(const std::string& name, int64_t def) const 
{
    return convertToInt(findSetting(name), endSettings(), "setting", name, def) ;
}

bool CommandLine::settingAsBool(const std::string& name, bool def) const 
{
    return convertToBool(findSetting(name), endSettings(), "setting", name, def) ;
}

double CommandLine::settingAsDbl(const std::string& name, double def) const 
{
    return convertToDbl(findSetting(name), endSettings(), "setting", name, def) ;
}


