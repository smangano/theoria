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

using Config = config::Config ;

std::unique_ptr<const config::Config> Bootstrap::loadConfig() const
{
    std::string filename = findConfig() ;
    config::TOMLConfigBuilder builder ;
    return builder.parse_file(filename) ;
}

std::string Bootstrap::findConfig() const
{
    const char * envPath = ::getenv(ENV_BOOTSTRAP_CONFIG_PATH) ;
    envPath = envPath ? envPath : "" ;
    std::string path = util::CommandLine::instance().settingAsStr(OPTION_BOOTSTRAP_CONFIG_PATH, envPath) ; 
    if (path == "")
        path = os::bootstrap_path() ;

    auto dirs = os::split_search_path(path) ;
    std::string filename ;
    for (auto dir : dirs) {
        filename = os::join_path(dir, "bootstrap.toml") ;
        if (os::exists(filename)) 
            break ;
        filename = "" ;
    }
    if (filename == "")
        throw RUNTIME_ERROR("boostrap.toml not found whils searching %s. Can't proceed.", path.c_str()) ; 
    return filename ;
}

void Bootstrap::createCoreComponents(ConstConfigList& componentConfigs, std::vector<Component*>& coreComponents)
{
    for (const Config* compConfig : componentConfigs) {
        Component * comp =_createCoreComp(*compConfig) ;
        coreComponents.push_back(comp) ;
    }
}

void Bootstrap::initializeCoreComponent(
    ConstConfigList& componentConfigs, 
    const std::vector<Component*>& coreComponents,
    std::unordered_map<CompId, Dependencies>& coreCompDeps )
{
    int numComps = componentConfigs.size() ;
    for (int i=0; i<numComps; ++i) {
        Dependencies&& deps = coreComponents[i]->init(*componentConfigs[i]) ;
        coreCompDeps[coreComponents[i]->id()] = std::move(deps) ;
    }
}

void Bootstrap::initializeAppLifeCycle(const std::vector<Component*>& coreComponents)
{
    for (Component* comp : coreComponents)
        comp->appLifeCycle(AppLifeCycle::INITIALIZED) ;
}

void Bootstrap::satisfyAndFinalize( std::unordered_map<CompId, Dependencies>& coreCompDeps)
{
    for (auto& compId_deps : coreCompDeps) {
        auto compDeps = Registry::instance().satisfy(compId_deps.second) ;
        Registry::instance().component(compId_deps.first)->finalize(compDeps) ;
    }
}

void Bootstrap::finalizeAppLifeCycle(const std::vector<Component*>& coreComponents)
{
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

//shake Theoria's booty, shake it's booty...
// LCOV_EXCL_START
void Bootstrap::boot(const config::Config& bootConfig)
{

    using Config = config::Config ;

    auto componentConfigs = 
        bootConfig.getChildren( [] (const Config* child) {return child->getAttrAsStr("kind", "Component") == "Component";} ) ;

    std::vector<Component*> coreComponents ;
    std::unordered_map<CompId, Dependencies> coreCompDeps ;

    //First create all core components
    createCoreComponents(componentConfigs, coreComponents) ;
    
    //Then initialize core components
    initializeCoreComponent(componentConfigs, coreComponents, coreCompDeps);

    //Fire INITIALIZED AppLifeCycle  Event
    initializeAppLifeCycle(coreComponents) ;

    //Satisfy deps and finalize core components
    satisfyAndFinalize(coreCompDeps) ;

    //Fire FINALIZED AppLifeCycle  Event
    finalizeAppLifeCycle(coreComponents) ;
}
// LCOV_EXCL_STOP


