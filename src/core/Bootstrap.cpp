#include <theoria/core/Bootstrap.h>
#include <theoria/core/Registry.h>
#include <theoria/core/Component.h>

#include <theoria/envvars.h>
#include <theoria/options.h>

#include <theoria/config/Config.h>
#include <theoria/config/TOMLConfigBuilder.h>
#include <theoria/except/except.h>
#include <theoria/os/os.h>
#include <theoria/util/CommandLine.h>


#include <cstdlib>
#include <string>

using namespace theoria ;
using namespace core ;

std::unique_ptr<config::Config> Bootstrap::loadConfig() const
{
    std::string filename = findConfig() ;
    config::TOMLConfigBuilder builder ;
    return std::unique_ptr<config::Config>(builder.parse_file(filename)) ;
}

std::string Bootstrap::findConfig() const
{
    std::string path = util::CommandLine::instance().settingAsStr(OPTION_BOOTSTRAP_CONFIG_PATH, ::getenv(ENV_BOOTSTRAP_CONFIG_PATH)) ; 
    if (path == "")
        path = os::bootstrap_path() ;

    auto dirs = os::split_search_path(path) ;
    std::string filename ;
    for (auto dir : dirs) {
        filename = os::join_path(dir, "bootstrap.toml") ;
        if (os::exists(filename)) 
            break ;
    }
    if (filename == "")
        throw RUNTIME_ERROR("boostrap.toml not found whils searching %s. Can't proceed.", path.c_str()) ; 
    return filename ;
}

//shake Theoria's booty, shake it's booty...
void Bootstrap::boot(config::Config& bootConfig)
{
    using Config = config::Config ;

    auto componentConfigs = 
        bootConfig.getChildren( [] (const Config* child) {return child->getAttrAsStr("kind", "Component") == "Component";} ) ;

    std::vector<Component*> coreComponents ;
    std::unordered_map<CompId, Dependencies> coreCompDeps ;

    //First create all core components
    for (const Config* compConfig : componentConfigs) {
        Component * comp =_createCoreComp(*compConfig) ;
        coreComponents.push_back(comp) ;
    }
    
    //Then initialize core components
    int numComps = componentConfigs.size() ;
    for (int i=0; i<numComps; ++i) {
        Dependencies&& deps = coreComponents[i]->init(*componentConfigs[i]) ;
        coreCompDeps[coreComponents[i]->id()] = std::move(deps) ;
    }
    //Fire INITITIALIZED AppLifeCycle  Event
    for (Component* comp : coreComponents)
        comp->appLifeCycle(AppLifeCycle::INITITIALIZED) ;

    //Satisfy deps and finalize core components
    for (auto& compId_deps : coreCompDeps) {
        auto compDeps = Registry::instance().satisfy(compId_deps.second) ;
        Registry::instance().component(compId_deps.first)->finalize(compDeps) ;
    }
    //Fire FINALIZED AppLifeCycle  Event
    for (Component* comp : coreComponents)
        comp->appLifeCycle(AppLifeCycle::FINALIZED) ;
}

Component *  Bootstrap::_createCoreComp(const config::Config& compConfig)
{
    TypeName compType = compConfig.getAttrAsStr("type", compConfig.name()) ;
    SubTypeName compSubType = compConfig.getAttrAsStr("subtype", "") ;
    Component * component = nullptr;
    if (compSubType == "")
        component = Registry::instance().createComponent(compType) ;
    else
        component = Registry::instance().createComponent(compType, compSubType) ; 
    return component ;
}


