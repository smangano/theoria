#include <theoria/util/CommandLine.h>
#include <theoria/core/Theoria.h>
#include <theoria/core/Registry.h>
#include <theoria/config/Config.h>
#include <theoria/options.h>

#include <iostream>
#include <stdexcept>

using namespace theoria ;

int main(int argc, const char ** argv) 
{
    try {

        core::Theoria theoria ;
        util::CommandLine cmdline(argc-1, &(argv[1])) ;

        if (cmdline.hasSetting(OPTION_HELP[OPTS_IDX_NAME])) 
        {
            if (cmdline.settingAsStr(OPTION_HELP[OPTS_IDX_NAME], "") != std::string("true"))
            {
                theoria.help(cmdline.settingAsStr(OPTION_HELP[OPTS_IDX_NAME], "")) ;
            }
            else
            {
                theoria.help() ;
            }
            return 0 ;
        }

        theoria.init() ;

        if (cmdline.hasSetting(OPTION_SHOW_CONFIG[OPTS_IDX_NAME]) || cmdline.hasSetting(OPTION_SHOW_CONFIG_ONLY[OPTS_IDX_NAME])) 
        {
            theoria.show_config(cmdline.hasSetting(OPTION_SHOW_CONFIG_ONLY[OPTS_IDX_NAME])) ;
        }

        if (!cmdline.hasSetting(OPTION_INIT_ONLY[OPTS_IDX_NAME]))
        {
            theoria.run() ;
            core::Registry::instance().dump(std::cout) ;
        }
    }

    catch (const std::runtime_error& ex) {
        std::cerr << ex.what() <<std::endl ;
    }
}
