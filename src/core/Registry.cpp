#include <theoria/core/Registry.h>
#include <theoria/core/Component.h>
#include <theoria/config/Config.h>
#include <theoria/except/except.h>

#include <algorithm>
#include <iostream>
#include <mutex>

using namespace theoria ;
using namespace core ;

using FactoryMap_iterator = Registry::FactoryMap_iterator ;

static std::mutex registry_lock ;

namespace 
{
    //For paractical purposes this should be fine as the upper bound of any reasonable name
    const std::string _HIGHEST =  { '\xff', '\xff', '\xff' , '\xff', '\xff', '\xff', '\xff', '\xff', '\xff' } ;
}

RegistryLock::RegistryLock()
{
    registry_lock.lock();
}

RegistryLock::~RegistryLock()
{
    registry_lock.unlock();
}

Registry::~Registry()
{
    for (auto pair : _components) {
        delete pair.second ;
    }
}

Registry& Registry::instance() 
{
    static Registry TheRegistry ;

    return TheRegistry ;
}

void Registry::reset()
{
    Registry::instance()._nextId = 0 ;
    Registry::instance()._factories.clear() ;
    //TODO: delete!
    Registry::instance()._components.clear() ;
    Registry::instance()._xref.clear() ;
}

void Registry::registerFactory(const std::string& type, ComponentFactory factory) 
{
    std::lock_guard<std::mutex> guard(registry_lock);
    _factories[std::make_pair(type,type)] = factory ;
}

void Registry::registerFactory(const TypeName& type, const SubTypeName& subtype, ComponentFactory factory) 
{
    std::lock_guard<std::mutex> guard(registry_lock);
    _factories[std::make_pair(type,subtype)] = factory ;
}

FactoryMap_iterator Registry::beginFact() 
{
    return _factories.begin() ;
}

FactoryMap_iterator Registry::endFact() 
{
    return _factories.end() ;   
}

FactoryMap_iterator Registry::findFact(const TypeName& type) 
{
    return _factories.lower_bound(std::make_pair(type, std::string())) ;
}

FactoryMap_iterator Registry::findfact(FactoryMap_iterator start, bool (*predicate)(FactoryMap::value_type& v))  
{
    return std::find_if(start, endFact(), predicate) ;
}

Component* Registry::createComponent(const Dependencies::Dependent& dep) noexcept 
{
    try { //locked
        std::lock_guard<std::mutex> guard(registry_lock);
        auto range = 
            std::make_pair(
                _factories.lower_bound(CompFactoryKey(dep.type, dep.subtype)),
                _factories.upper_bound(CompFactoryKey(dep.type, _HIGHEST))
            ) ;
        if (range.first == range.second)
            return nullptr ; //No factory available to satisy dep
        auto& iter = range.first ;
        auto& key = iter->first ;
        if (dep.subtype != "" && dep.subtype != key.second) {    
            return nullptr ; //A default or strict dependency could not be satified
        }
        return _createComponent(range.first) ;
    } //unlocked
    catch (...) {
        return nullptr ;
    }
}

Component* Registry::createComponent(const std::string& type, int allow_ambiguity) 
{
    { //locked
        std::lock_guard<std::mutex> guard(registry_lock);
        auto range = 
            std::make_pair(
                _factories.lower_bound(CompFactoryKey(type, std::string())),
                _factories.upper_bound(CompFactoryKey(type, _HIGHEST))
            ) ;
        if (range.first != range.second) {
            if (distance(range.first, range.second)  == 1)
            {
                return _createComponent(range.first) ;
            }
            else
            {
                //Check for usage
                for (auto iter = range.first; iter != range.second; ++iter) {
                    if (_xref.find(iter->first) != _xref.end()) {
                        return _createComponent(iter) ;
                    }
                }
                //No component's of type have been used so look for the default
                auto def = _factories.find(std::make_pair(type, type)) ;
                if (def != _factories.end()) {
                    return _createComponent(def) ;
                }
                if (allow_ambiguity) {
                    //No compelling reason to pick one so use first
                    return _createComponent(range.first) ;
                }
                else {
                    throw RUNTIME_ERROR("Registry failed to create component of type [%s] : Ambiguous: [%d] choices",
                                         type.c_str(), std::distance(range.first,range.second)) ;
                }
            }
        } 
    } //unlocked
    throw RUNTIME_ERROR("Registry failed to create component of type [%s] : Not Registered", type.c_str()) ;
}

Component* Registry::createComponent(const TypeName& type, const SubTypeName& subtype) 
{
    { //locked
        std::lock_guard<std::mutex> guard(registry_lock);
        auto iter = _factories.find(std::make_pair(type, subtype)) ;
        if (iter != _factories.end())
            return _createComponent(iter) ;
    } //unlocked

    throw RUNTIME_ERROR("Registry failed to create component of type [%s] and subtype [%s]: Not Registered",
                        type.c_str(), 
                        subtype.c_str()
                       ) ;
}

Component* Registry::_createComponent(FactoryMap_iterator iter) 
{
    Component* comp = iter->second(_nextId) ;
    if (!comp) 
        throw RUNTIME_ERROR("Registry failed to create component of type [%s] subtype [%s] : Factory returned nullptr",
                            iter->first.first.c_str(),
                            iter->first.second.c_str()) ;
    comp->setName(iter->first.first + ":" + iter->first.second) ;
    _components[comp->id()] = comp ;
    _xref.insert(std::make_pair(iter->first, _nextId++)) ;
    return comp ;
}

void Registry::dump(std::ostream& stream) const 
{
    stream << "Factories {\n" ;
    for (auto f : _factories) {
        stream << "(" << f.first.first << ", " << f.first.second <<  ") ->" << f.second << "\n" ;
    }
    stream << "}\n" ;

    stream << "CompXRefs{\n" ;
    for (auto f : _xref) {
        stream << "(" << f.first.first << ", " << f.first.second <<  ") ->" << f.second << "\n" ;
    }
    stream << "}\n" ;

    stream << std::endl ;
}

void Registry::_setBootstrapConfig(std::unique_ptr<config::Config>& pBSConfig) 
{
    if (_bootstrapConfig)
        throw RUNTIME_ERROR("Bootstrap Config already set.") ;
    _bootstrapConfig = pBSConfig.release() ;
}

void Registry::_setAppConfig(std::unique_ptr<config::Config>& pAppConfig) 
{
    if (_appConfig)
        throw RUNTIME_ERROR("App Config already set.") ;
    _appConfig = pAppConfig.release() ;
}

const config::Config& Registry::bootConfig() const
{
    if (!_bootstrapConfig)
        throw RUNTIME_ERROR("Bootstrap Config is not set.") ;

    return *_bootstrapConfig ;
}

const config::Config& Registry::appConfig() const 
{
    if (!_appConfig)
        throw RUNTIME_ERROR("App Config is not set.") ;
    return *_appConfig ;
}

util::Maybe<Component>  Registry::component(const TypeName& type) 
{
    auto iter = _xref.lower_bound(CompFactoryKey(type, std::string())) ;
    if (iter != _xref.end() && iter->first.first == type)
        return util::Maybe<Component>(component(iter->second)) ;
    return util::Maybe<Component>(nullptr) ;
}

util::Maybe<Component>  Registry::component(const TypeName& type, const SubTypeName& subtype) 
{
    auto iter = _xref.find(CompFactoryKey(type, subtype)) ;
    if (iter != _xref.end())
        return util::Maybe<Component>(component(iter->second)) ;
    return util::Maybe<Component>(nullptr) ;
}

util::Maybe<Component> Registry::component(const Dependencies::Dependent& dep)
{
    if (dep.subtype == "")
        return component(dep.type) ;
    else
        return component(dep.type, dep.subtype) ;
}

std::vector<Component*> Registry::satisfy(const Dependencies& deps, CompId id) 
{
    std::vector<Component*> satisfaction ;
    for(const Dependencies::Dependent& dep : deps) 
    {
        try
        {
            auto comp = component(dep) ;
            if (!comp && dep.required()) {
                //Attempt to satsify by creating it on the fly    
                comp.reset(createComponent(dep)) ;  
                *comp ; //May trigger exception as required dep could not be satisfied
            }
            satisfaction.push_back(comp.get()) ;
        }
        catch (const std::runtime_error& err) {
            //"I can't get no satisfaction
            // I can't get me no satisfaction
            // And I try and I try and I try t-t-t-t-try try
            // I can't get no I can't get me no"
            //-- The Rolling Stones.
            std::string name = id >= 0 ? component(id)->name() : std::string("Unknown") ;
            throw RUNTIME_ERROR("Dependency  [%s] for component [%s#%d] could not be satisfied",
                dep.str().c_str(), name.c_str(), id) ;
        }
    }
    return satisfaction ;
}
