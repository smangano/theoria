
#include <theoria/core/Registry.h>
#include <theoria/except/except.h>
#include <mutex>

using namespace theoria ;
using namespace core ;

static std::mutex lock ;

Registry& Registry::instance() 
{
    static Registry TheRegistry ;

    return TheRegistry ;
}

void Registry::registerFactory(const std::string& type, ComponentFactory factory) 
{
    std::lock_guard<std::mutex> guard(lock);
    _factories[type] = factory ;
}

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

