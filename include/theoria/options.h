#pragma once

#include <string>

namespace theoria
{
    static const std::string dashdash("--") ;

    /*
     * --bootstrap_path :  The path to search for bootstrap.toml 
     */
    const char * const OPTION_BOOTSTRAP_CONFIG_PATH = "bootstrap_path" ;

    /*
     * --help : Display help
     */
    const char * const OPTION_HELP = "help" ;

    /*
     * --init_only : Initialize theoria by processing bootstrap and then exit
     */
    const char * const OPTION_INIT_ONLY = "init_only" ;

    /*
     * --show_config : Display configuration after resolved
     * --show_config_only : Same but exit afterward
     */
    const char * const OPTION_SHOW_CONFIG = "show_config" ;
    const char * const OPTION_SHOW_CONFIG_ONLY = "show_config_only" ;

    inline const char * OPT(std::string& buf, const std::string& opt) {buf = dashdash+opt; return buf.c_str();}

} 

