#pragma once

#include <theoria/core/Component.h>
#include <theoria/core/primitives.h>
#include <theoria/core/Dependencies.h>
#include <theoria/core/RegisterThis.h>
#include <theoria/util/visibility.h>

namespace theoria { namespace core {

/**
 * A component responsible for wiring a ConfigBuilder to one or more Resolvers 
 * based on configuration
 */
class ConfigVarResolverBuilderComp : public Component
{
public:

    /**
     * Component constructor
     */
    ConfigVarResolverBuilderComp(CompId id) :
        Component(id) {} 

    /**
     *Factory for this component
     */
    static Component* factory(CompId id) ; 

    /**
     * Uses configuration of the form:
     *  ConfigVarResolverBuilder
     *      [Resolver1, Resolver2, ...]
     *  to build Dependencies between the default ConfigBuilder and the ordered sequence of resolvers.
     */
	Dependencies init(const config::Config& config) override;

    /**
     * Wires up Builder and resolvers in order presented in dependencies
     */
    void finalize(const std::vector<Component*>& dependencies) override ;

    /**
     * When state is FINALIZED this component releases itself from Registry as it serves
     * no purpose afterward
     */
    void appLifeCycle(AppLifeCycle state) override ;

public:

    /**
     * Register factory
     */
    static DLL_PUBLIC RegisterThis<ConfigVarResolverBuilderComp> rt ; 
} ;


/**
 * Component wrapper to a EnvVarResolver
 */
class EnvVarResolverComp : public Component
{
public:

    /**
     * Component constructor
     */
    EnvVarResolverComp(CompId id) : Component(id) {}

    /**
     * Factory for this component
     */
    static Component* factory(CompId id) ; 

    /**
     * Used to acquire underlying resolver
     */
    Component* acquire(const std::type_info& typeInfo, void** dest) override ;
    
   
    /**
     * Register factory
     */
    static DLL_PUBLIC RegisterThis<EnvVarResolverComp> rt ; 
} ;

/**
 * Component wrapper to CmdLineResolver
 */
class CmdLineResolverComp : public Component
{
public:

    /**
     * Component constructor
     */
    CmdLineResolverComp(CompId id) : Component(id) {}

    /**
     * Factory for this component
     */
    static Component* factory(CompId id) ; 

    /**
     * Used to acquire underlying resolver
     */
    Component* acquire(const std::type_info& typeInfo, void** dest) override ;
   
    /**
     * Register factory
     */
    static DLL_PUBLIC RegisterThis<CmdLineResolverComp> rt ; 
} ;

/**
 * Component wrapper to a AppConfigResolver
 */
class AppConfigResolverComp : public Component
{
public:

    /**
     * Component constructor
     */
    AppConfigResolverComp(CompId id) : Component(id) {}

    /**
     * Factory for this component
     */
    static Component* factory(CompId id) ; 

    /**
     * Used to acquire underlying resolver
     */
    Component* acquire(const std::type_info& typeInfo, void** dest) override ;
    
   
    /**
     * Register factory
     */
    static DLL_PUBLIC RegisterThis<AppConfigResolverComp> rt ; 
} ;

/**
 * Component wrapper to TOMLConfigBuilder
 */
class TOMLConfigBuilderComp : public Component
{
public:

    /**
     * Component constructor
     */
    TOMLConfigBuilderComp(CompId id) : Component(id) {}

    /**
     * Factory for this component
     */
    static Component* factory(CompId id) ; 

    /**
     * Used to acquire underlying resolver
     */
    Component* acquire(const std::type_info& typeInfo, void** dest) override ;
    
    /**
     * Register factory
     */
    static DLL_PUBLIC RegisterThis<TOMLConfigBuilderComp> rt ; 
} ;

}}
