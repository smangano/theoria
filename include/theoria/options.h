#pragma once

#include <string>

namespace theoria
{
    static const std::string dashdash("--") ;

    /*
     * --bootstrap_path :  The path to search for bootstrap.toml 
     */

    const int OPTS_IDX_NAME = 0;
    const int OPTS_IDX_HELP = 1;
    const int OPTS_IDX_TYPE = 2;
    const int OPTS_IDX_EXAMPLE = 3;
    const int OPTS_IDX_CHOICES = 4;

    /*
     * Format of these constant arrays
     *
     * const char * const OPT = {"name", "help string", "type", "example", "choices"} 
     * where:
     *          name - name of the --option
     *          help string - displayed when user requests help
     *          type - switch (option takes no args), string, int, double, date (YYYY-MM-DD) , datetime (YYYY-MM-DD HH:MM:SS), interval with units (e.g. 10.5h, 1s, 100ms, 700us)
     */

    const char * const OPTION_BOOTSTRAP_CONFIG_PATH[] = 
        {"bootstrap_path" , "Path to bootstrap.ini", "string", "--bootstrap_path /home/jsmith/theoria/boot.toml", ""} ;

    const char * const OPTION_HELP[] = 
        {"help", "Display help for theoria or a particular option", "string", "--help or -help option", ""} ;

    const char * const OPTION_INIT_ONLY[] = 
        {"init_only", "Used to test changes to bootstrap.toml", "switch", "", ""}  ;

    const char * const OPTION_SHOW_CONFIG[] = 
        {"show_config", "Displays config file after variable resolution", "switch", "", ""} ;

    const char * const OPTION_SHOW_CONFIG_ONLY[] = 
        {"show_config_only", "Exits after displaying config file after variable resolution", "switch", "", ""} ;

    const char * const OPTION_PAUSE_AFTER_APP_INIT[] = 
        {"pause_after_app_init", "Intializes all app components and pauses waiting for operator iteraction", "switch", "", ""} ;

    const char * const OPTION_PAUSE_AFTER_APP_FINALIZE[] = 
        {"pause_after_app_finalize", "Finalizes all app components and pauses waiting for operator interaction", "switch", "", ""} ;

    const char * const OPTION_DEFAULT_LOG_LEVEL[] = 
        {"default_log_level", "Set the default logging level", "string", "--default_log_level DEBUG", "DEBUG,INFO, WARN, ERROR, CRITICAL" } ;

    const char * const WEB_SERVICE_PORT[] = 
        {"ws_port", "Start web service for ineractivity and use this port", "int", "-ws_port 8900", ""} ; 


    /**
     * Build a --option string
     */
    inline const char * OPT(std::string& buf, const std::string& opt) {buf = dashdash+opt; return buf.c_str();}

} 

