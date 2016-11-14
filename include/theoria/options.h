#pragma once

#include <string>

namespace theoria
{
    static const std::string dashdash("--") ;

    const int OPTS_IDX_NAME = 0;
    const int OPTS_IDX_HELP = 1;
    const int OPTS_IDX_ARG = 2 ;
    const int OPTS_IDX_TYPE = 3;
    const int OPTS_IDX_EXAMPLE = 4;
    const int OPTS_IDX_CHOICES = 5;
    const int OPTS_SZ = 6 ;

    typedef const char * OPTS_TYPE[OPTS_SZ] ;
    extern const OPTS_TYPE ALL_OPTIONS[] ;
    extern const char * const * OPTION_BOOTSTRAP_CONFIG_PATH ;
    extern const char * const * OPTION_DEFAULT_LOG_LEVEL ;
    extern const char * const * OPTION_HELP ;
    extern const char * const * OPTION_INIT_ONLY ;
    extern const char * const * OPTION_PAUSE_AFTER_APP_INIT ;
    extern const char * const * OPTION_PAUSE_AFTER_APP_FINALIZE ;
    extern const char * const * OPTION_SHOW_CONFIG ;
    extern const char * const * OPTION_SHOW_CONFIG_ONLY ;
    extern const char * const * WEB_SERVICE_PORT ;

    /**
     * Build a --option string
     */
    inline const char * OPT(std::string& buf, const std::string& opt) {buf = dashdash+opt; return buf.c_str();}
    inline const char * OPT(std::string& buf, const char* const * opt) {return OPT(buf, opt[OPTS_IDX_NAME]); }
    inline const char * OPT_ARG(std::string& buf, const char* const * opt) {return opt[OPTS_IDX_ARG];}

} 

