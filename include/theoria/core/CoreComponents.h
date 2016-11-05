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
    void finalize(std::vector<Component*>& dependencies) override ;
    void appLifeCycle(AppLifeCycle state) override ;

public:

    static DLL_PUBLIC RegisterThis<ConfigVarResolverBuilderComp> rt ; 
} ;

}}
