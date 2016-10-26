#pragma once

#include <theoria/core/primitives.h>
#include <theoria/core/Dependencies.h>

#include <memory>
#include <string>

namespace theoria { 

namespace config {

    class Config ;

} //namespace theoria::config

namespace core {

/* Bootstrap is responsible for setting up the components that are required by theoria to implement its
 * feature set. As some features may be enabled, disabled or tweaked by user a bootstrap.toml file
 * is used to configure Bootstrap
 */

class Bootstrap
{
public:

    /*
     * Load bootstrap config
     */
    std::unique_ptr<config::Config> loadConfig() const ;

    /*
     * Find the location of the bootstrap.toml file. This location 
     * is configurable via command line, env var and OS specific locations.
     * See theoria docs for details
     */
    std::string findConfig() const ;

    /*
     * Bootstrap theoria by processing the bootstrap config and
     * registering the required core components
     */
    void boot(config::Config& bootConfig) ;

    Component *  _createCoreComp(const config::Config& compConfig) ;
    Dependencies _initCoreComp(Component * component) ;

} ;

}} //namespace theoria::core
