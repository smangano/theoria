#include <theoria/core/CoreComponents.h>
#include <theoria/core/AppConfigResolver.h>
#include <theoria/config/Builder.h>
#include <theoria/config/Config.h>
#include <theoria/config/Resolve.h>
#include <theoria/config/TOMLConfigBuilder.h>

using namespace theoria;
using namespace core;

DLL_PUBLIC RegisterThis<ConfigVarResolverBuilderComp> ConfigVarResolverBuilderComp::rt("ConfigVarResolverBuilder") ; 

Component* ConfigVarResolverBuilderComp::factory(CompId id) 
{
    return new ConfigVarResolverBuilderComp(id) ;
}

Dependencies ConfigVarResolverBuilderComp::init(const config::Config& config) 
{
    using Config = config::Config ;
    using ConfigArray = config::ConfigArray ;
    Config::ConstConfigList children = config.getChildren() ;
    if (!children[0]->isArray())
        throw RUNTIME_ERROR("ConfigVarResolverBuilder expects a array of Resolver specifications.") ;
    const ConfigArray& array = static_cast<const ConfigArray&>(*children[0]) ;
    Dependencies deps ;
    //Specify a loose dependency to what ever config builder is used. This will allow injection of
    //the resolver chain during finalization
    deps.loose("ConfigBuilder") ;
    //Now parse the configured resolvers as strict dependencies
    for (auto resolver : array) {
        std::string type = resolver->getAttrAsStr("type") ; 
        std::string subtype = resolver->getAttrAsStr("subtype") ;
        bool optional = resolver->getAttrAsBool("optional", false) ;
        if (type == "") 
            throw RUNTIME_ERROR("ConfigVarResolverBuilder found resolver with no type attribute.") ;
        deps.strict(type, subtype, optional) ;
    }
    return deps ; 
}

/**
 * The ConfigVarResolverBuilderComp job is to wire up ConfigBuilder to 
 * chain of resolvers. 
 */
void ConfigVarResolverBuilderComp::finalize(const std::vector<Component*>& dependencies) 
{
    
    using ConfigBuilder = config::ConfigBuilder ;
    using ConfigVariableResolver = config::ConfigVariableResolver ;
    std::string requestor = "ConfigVarResolverBuilderComp::finalize" ;
    ConfigBuilder* builder = dependencies[0]->cast<ConfigBuilder>(requestor) ;
    int sz = dependencies.size() ;
    ConfigVariableResolver* prev = nullptr ;
    for (int i=1; i<sz; ++i) {
        ConfigVariableResolver* resolver = dependencies[i]->cast<ConfigVariableResolver>(requestor) ;
        if (!prev) {
            builder->setResolver(resolver) ;
            prev = resolver ;
        }
        else {
            prev->append(resolver);
            prev = resolver ;
        }
        Registry::instance().addDep(dependencies[i-1], dependencies[i]); 
    }
}

void ConfigVarResolverBuilderComp::appLifeCycle(AppLifeCycle state) 
{
    //After finalization this component has no responsibility so it releases
    //itself
    if (state == AppLifeCycle::FINALIZED) 
        Registry::instance().release(this) ;
}

// 
// EnvVarResolverComp
//
RegisterThis<EnvVarResolverComp> EnvVarResolverComp::rt("ConfigVariableResolver", "EnvVarResolver") ;

Component* EnvVarResolverComp::factory(CompId id) 
{
    return new EnvVarResolverComp(id) ;
}

Component* EnvVarResolverComp::acquire(const std::type_info& typeInfo, void** dest)
{
    static config::EnvVarResolver resolver ;
    *dest = nullptr ;
    if (typeInfo == typeid(config::ConfigVariableResolver))
        *dest = &resolver ;
    return nullptr ;
}

// 
// CmdLineResolverComp
//
RegisterThis<CmdLineResolverComp> CmdLineResolverComp::rt("ConfigVariableResolver", "CmdLineResolver") ;

Component* CmdLineResolverComp::factory(CompId id) 
{
    return new CmdLineResolverComp(id) ;
}

Component* CmdLineResolverComp::acquire(const std::type_info& typeInfo, void** dest)
{
    static config::CmdLineResolver resolver ;
    *dest = nullptr ;
    if (typeInfo == typeid(config::ConfigVariableResolver))
        *dest = &resolver ;
    return nullptr ;
}

//
// AppConfigResolverComp
//
RegisterThis<AppConfigResolverComp> AppConfigResolverComp::rt("ConfigVariableResolver", "AppConfigResolver") ;

Component* AppConfigResolverComp::factory(CompId id) 
{
    return new AppConfigResolverComp(id) ;
}

Component* AppConfigResolverComp::acquire(const std::type_info& typeInfo, void** dest)
{
    static AppConfigResolver resolver ;
    *dest = nullptr ;
    if (typeInfo == typeid(config::ConfigVariableResolver))
        *dest = &resolver ;
    return nullptr ;
}

// 
// TOMLConfigBuilderComp
//
RegisterThis<TOMLConfigBuilderComp> TOMLConfigBuilderComp::rt("ConfigBuilder", "TOMLConfigBuilder") ;

Component* TOMLConfigBuilderComp::factory(CompId id) 
{
    return new TOMLConfigBuilderComp(id) ;
}

Component* TOMLConfigBuilderComp::acquire(const std::type_info& typeInfo, void** dest)
{
    static config::TOMLConfigBuilder builder ;
    *dest = nullptr ;
    if (typeInfo == typeid(config::ConfigBuilder))
        *dest = &builder ;
    return nullptr ;
}

