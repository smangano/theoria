#include <theoria/core/Theoria.h>
#include <theoria/core/Bootstrap.h>
#include <theoria/core/Registry.h>
#include <theoria/config/Config.h>

#include <theoria/error.h>
#include <theoria/options.h>

#include <iomanip>
#include <cstdlib>

using namespace theoria;
using namespace core;

namespace theoria { 
    /*
     * Format of these constant arrays
     *
     * const char * const OPT = {"name", "help string", "optarg", "type", "example", "choices"} 
     * where:
     *          name - name of the --option
     *          help string - displayed when user requests help
     *          type - switch (option takes no args), string, int, double, date (YYYY-MM-DD) , datetime (YYYY-MM-DD HH:MM:SS), interval with units (e.g. 10.5h, 1s, 100ms, 700us)
     */
     extern const OPTS_TYPE ALL_OPTIONS[] = {

        {"bootstrap_path",    "Path to bootstrap.ini", "path", "string", "--bootstrap_path /home/jsmith/theoria/boot.toml", ""}, 

        {"default_log_level", "Set the default logging level", "level", "string", "--default_log_level DEBUG", "DEBUG,INFO, WARN, ERROR, CRITICAL" }, 

        {"help",              "Display help for theoria or a particular option", "[option]",  "string", "--help or -help option", ""}, 

        {"init_only",         "Used to test changes to bootstrap.toml", "", "switch", "", ""},

        {"pause_after_app_init", "Intializes all app components and pauses waiting for operator iteraction", "", "switch", "", ""}, 

        {"pause_after_app_finalize", "Finalizes all app components and pauses waiting for operator interaction", "", "switch", "", ""}, 

        {"show_config", "Displays config file after variable resolution", "", "switch", "", ""}, 

        {"show_config_only", "Exits after displaying config file after variable resolution", "", "switch", "", ""}, 

        {"ws_port", "Start web service for ineractivity and use this port", "port", "int", "-ws_port 8900", ""},  

        } ;

    const char * const * OPTION_BOOTSTRAP_CONFIG_PATH = ALL_OPTIONS[0];
    const char * const * OPTION_DEFAULT_LOG_LEVEL = ALL_OPTIONS[1] ;
    const char * const * OPTION_HELP = ALL_OPTIONS[2] ;
    const char * const * OPTION_INIT_ONLY = ALL_OPTIONS[3] ;
    const char * const * OPTION_PAUSE_AFTER_APP_INIT = ALL_OPTIONS[4] ;
    const char * const * OPTION_PAUSE_AFTER_APP_FINALIZE = ALL_OPTIONS[5];
    const char * const * OPTION_SHOW_CONFIG  = ALL_OPTIONS[6];
    const char * const * OPTION_SHOW_CONFIG_ONLY = ALL_OPTIONS[7];
    const char * const * WEB_SERVICE_PORT = ALL_OPTIONS[8] ;
}

namespace {

struct Options
{
} ;    

std::ostream& operator << (std::ostream& os, const Options& options) 
{
    int i = 0 ;
    for (auto opt : theoria::ALL_OPTIONS) {
        std::string buf1, buf2 ;
        os << "[" << OPT(buf1, opt) << OPT_ARG(buf2,opt) << "] " ;  
        i++ ;
        if (i % 3 == 0) 
            os << "\n               " ;
    }
    return os ;
}

struct Arg
{
    Arg(const char * const *opt_) : opt(opt_) {}

    const char * const *opt ;
};

std::ostream& operator << (std::ostream& os, const  Arg& arg)
{
    if (arg.opt[OPTS_IDX_ARG] != std::string())
        os << "<" << arg.opt[OPTS_IDX_ARG] << ">" ;
    return os ;
}

} //namespace

Theoria::Theoria()
{
}

void Theoria::help(std::ostream& out)
{
    out << "usage: theoria " << Options() << "appconfigName " << "[-- [user settings]]\n" << std::endl ;
    for (auto opt : ALL_OPTIONS) 
        help(opt, out) ;
}

void Theoria::help(const std::string& optionName, std::ostream& out) 
{
    int nopts = sizeof(ALL_OPTIONS)/sizeof(ALL_OPTIONS[0]) ;
    auto iter = std::find_if(&ALL_OPTIONS[0], &ALL_OPTIONS[nopts], 
                    [optionName](const char * const * opt) {return optionName == opt[OPTS_IDX_NAME];}) ;
    if (iter == &ALL_OPTIONS[nopts]) {
        out << "'" << optionName << "' is not a valid theoria option" << std::endl ;
    }
    else
    {
        help(*iter, out) ;
    }
}

void Theoria::help(const char * const *opt, std::ostream& out) 
{
    out << opt[OPTS_IDX_NAME] << " " << Arg(opt) << "\n" ;
    out << std::setw(4) << std::setiosflags(std::ios::left) << opt[OPTS_IDX_HELP] << "\n" ;
    if (opt[OPTS_IDX_EXAMPLE] != std::string()) 
         out <<  std::setw(4) << std::setiosflags(std::ios::left) << " " << "Example: " << opt[OPTS_IDX_EXAMPLE] << "\n" ;
    if (opt[OPTS_IDX_CHOICES] != std::string()) 
         out << std::setw(4) << std::setiosflags(std::ios::left) << " " << "Choices: " << opt[OPTS_IDX_CHOICES] << "\n" ;
    out << "\n" ;
}

void Theoria::show_config(bool exit, std::ostream& out) 
{
    Registry::instance().appConfig().toTOML(out) ;
    if (exit)
    {
        out << "\nExiting theoria due to user option" << std::endl ;
        std::exit(0) ;
    }
}

void Theoria::init()
{
    Bootstrap bs ;    

    try {
        auto bsConfig = bs.loadConfig() ;
        bs.boot(*bsConfig) ;
        Registry::instance()._setBootstrapConfig(bsConfig) ;
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl ; 
        std::exit(ERR_BOOTSTRAP) ;
    }

    //At this point registry should have a ConfigParser Component

}

void Theoria::run()
{
}

