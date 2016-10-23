#include <theoria/core/Theoria.h>
#include <theoria/core/Bootstrap.h>
#include <theoria/core/Registry.h>
#include <theoria/config/Config.h>

#include <theoria/error.h>

#include <iostream>
#include <cstdlib>

using namespace theoria;
using namespace core;

Theoria::Theoria()
{
}

void Theoria::help()
{
    std::cout << "TODO: help system" << std::endl ;
}

void Theoria::show_config(bool exit) 
{
    Registry::instance().appConfig().toTOML(std::cout) ;
    if (exit)
    {
        std::cout << "\nExiting theoria due to user option" << std::endl ;
        std::exit(0) ;
    }
}

void Theoria::init()
{
    Bootstrap bs ;    

    try {
        auto bsConfig = bs.loadConfig() ;
        Registry::instance()._setBootstrapConfig(bsConfig) ;
        bs.boot() ;
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

