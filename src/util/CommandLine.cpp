#include <theoria/util/CommandLine.h>
#include <theoria/except/except.h>

#include <cstring>

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

