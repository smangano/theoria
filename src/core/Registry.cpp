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

Component* Registry::createComponent(const std::string& type, bool allow_ambiguity) 
{
    { //locked
        std::lock_guard<std::mutex> guard(registry_lock);
        auto range = _factories.equal_range(std::make_pair(type, std::string())) ;
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

Component* Registry::_createComponent(FactoryMap_iterator iter) 
{
    Component* comp = iter->second(_nextId) ;
    if (!comp) 
        throw RUNTIME_ERROR("Registry failed to create component of type [%s] subtype [%s] : Factory returned nullptr",
                            iter->first.first.c_str(),
                            iter->first.second.c_str()) ;
    _xref.insert(std::make_pair(iter->first, _nextId++)) ;
    return comp ;
}

