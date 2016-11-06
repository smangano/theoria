#pragma once

#include <theoria/core/Component.h>
#include <theoria/core/primitives.h>
#include <theoria/core/Dependencies.h>
#include <theoria/core/RegisterThis.h>
#include <theoria/util/visibility.h>

namespace theoria { namespace core {

class ConfigVarResolverBuilderComp : public Component
{
public:

    
    ConfigVarResolverBuilderComp(CompId id) :
        Component(id) {} 

    static Component* factory(CompId id) ; 

	Dependencies init(const config::Config& config) override;
    void finalize(const std::vector<Component*>& dependencies) override ;
    void appLifeCycle(AppLifeCycle state) override ;

public:

    static DLL_PUBLIC RegisterThis<ConfigVarResolverBuilderComp> rt ; 
} ;


class EnvVarResolverComp : public Component
{
public:

    EnvVarResolverComp(CompId id) : Component(id) {}

    static Component* factory(CompId id) ; 
    Component* acquire(const std::type_info& typeInfo, void** dest) override ;
    
    static DLL_PUBLIC RegisterThis<EnvVarResolverComp> rt ; 
} ;

class CmdLineResolverComp : public Component
{
public:

    CmdLineResolverComp(CompId id) : Component(id) {}

    static Component* factory(CompId id) ; 
    Component* acquire(const std::type_info& typeInfo, void** dest) override ;
    
    static DLL_PUBLIC RegisterThis<CmdLineResolverComp> rt ; 
} ;


class AppConfigResolverComp : public Component
{
public:

    AppConfigResolverComp(CompId id) : Component(id) {}

    static Component* factory(CompId id) ; 
    Component* acquire(const std::type_info& typeInfo, void** dest) override ;
    
    static DLL_PUBLIC RegisterThis<AppConfigResolverComp> rt ; 
} ;

class TOMLConfigBuilderComp : public Component
{
public:

    TOMLConfigBuilderComp(CompId id) : Component(id) {}

    static Component* factory(CompId id) ; 
    Component* acquire(const std::type_info& typeInfo, void** dest) override ;
    
    static DLL_PUBLIC RegisterThis<TOMLConfigBuilderComp> rt ; 
} ;

}}
