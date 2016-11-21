/*
 * Theoria - A C++ Application Container for the Real and Virtual World
 *
 * Copyright (c) 2016 theoriacpp.com
 * 
 * This library is released under the Apache License v. 2.0. See LICENSE in top level directory of this project.
 * or https://github.com/smangano/theoria/blob/master/LICENSE 
 */

#pragma once

#include <theoria/core/primitives.h>
#include <theoria/core/Dependencies.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace theoria { 

namespace config {

    class Config ;

} //namespace theoria::config

namespace core {

/**
 * Bootstrap is responsible for setting up the components that are required by theoria to implement its
 * feature set. As some features may be enabled, disabled or tweaked by user a bootstrap.toml file
 * is used to configure Bootstrap
 */

class Bootstrap
{
public:

    using ConstConfigList = std::vector<const config::Config*> ;

    /**
     * Load bootstrap config
     */
    std::unique_ptr<const config::Config> loadConfig() const ;

    /**
     * Find the location of the bootstrap.toml file. This location 
     * is configurable via command line, env var and OS specific locations.
     * See theoria docs for details
     */
    std::string findConfig() const ;

    /**
     * Bootstrap theoria by processing the bootstrap config and
     * registering the required core components
     */
    void boot(const config::Config& bootConfig) ;

/*private - below are undocumented interfaces that should be treated as private but are public to facilitate unit-testing*/

    Component *  _createCoreComp(const config::Config& compConfig) ;
    Dependencies _initCoreComp(Component * component) ;
    
    void createCoreComponents(ConstConfigList& componentConfigs, std::vector<Component*>& coreComponents) ;
    void initializeCoreComponent(ConstConfigList& componentConfigs, const std::vector<Component*>& coreComponents, std::unordered_map<CompId, Dependencies>& coreCompDeps ) ;
    void initializeAppLifeCycle(const std::vector<Component*>& coreComponents) ;
    void satisfyAndFinalize( std::unordered_map<CompId, Dependencies>& coreCompDeps) ;
    void finalizeAppLifeCycle(const std::vector<Component*>& coreComponents) ;

} ;

}} //namespace theoria::core
