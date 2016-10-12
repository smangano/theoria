
#include <theoria/core/Registry.h>
#include <theoria/except/except.h>

#include <algorithm>
#include <mutex>

using namespace theoria ;
using namespace core ;

using FactoryMap_iterator = Registry::FactoryMap_iterator ;

static std::mutex registry_lock ;

RegistryLock::RegistryLock()
{
    registry_lock.lock();
}

RegistryLock::~RegistryLock()
{
    registry_lock.unlock();
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

#if 0
Component* Registry::createComponent(const std::string& type) 
{
    Component* comp = nullptr ;

    { //locked
        std::lock_guard<std::mutex> guard(lock);
        FactoryMap::iterator iter = _factories.find(type) ;
        if (iter != _factories.end()) {
            CompId id = _nextId++ ;
           _components[id] = comp = iter->second(id) ;
        } 
        else {
            throw RUNTIME_ERROR("Registry failed to create component of type [%s] : Not Registered", type.c_str()) ;
        }
    } //unlocked
    
    if (!comp) {
        throw RUNTIME_ERROR("Registry Failed to create component of type [%s]. Factory Failed", type.c_str()) ;
    }
   
    return comp ;    
}
#endif 
