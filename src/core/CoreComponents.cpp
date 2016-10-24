#include <theoria/core/Component.h>
#include <theoria/core/Dependencies.h>
#include <theoria/core/RegisterThis.h>
#include <theoria/core/primitives.h>
#include <theoria/config/Builder.h>
#include <theoria/config/Config.h>
#include <theoria/config/Resolve.h>

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

private:

    static RegisterThis<ConfigVarResolverBuilderComp> rt ; 
} ;

RegisterThis<ConfigVarResolverBuilderComp> 
    ConfigVarResolverBuilderComp::rt {"ConfigVarResolverBuilder", "ConfigVarResolverBuilder"} ; 

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
void ConfigVarResolverBuilderComp::finalize(std::vector<Component*>& dependencies) 
{
    
    using ConfigBuilder = config::ConfigBuilder ;
    using ConfigVariableResolver = config::ConfigVariableResolver ;
    std::string requestor = "ConfigVarResolverBuilderComp::finalize" ;
    ConfigBuilder* builder = dependencies[0]->bind<ConfigBuilder>(requestor) ;
    int sz = dependencies.size() ;
    ConfigVariableResolver* prev = nullptr ;
    for (int i=1; i<sz; ++i) {
        ConfigVariableResolver* resolver = dependencies[i]->bind<ConfigVariableResolver>(requestor) ;
        if (!prev) {
            builder->setResolver(resolver) ;
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

}} //namespace theoria::core
