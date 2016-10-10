#include <theoria/core/primitives.h>
#include <theoria/util/densemap.h>

#include <string>

#include <unordered_map>

namespace theoria { namespace core {

class Component ;


class Registry
{
private:

    using FactoryMap = std::unordered_map<TypeName, ComponentFactory> ;
    using ComponentMap = util::densemap<CompId, Component*> ;

    Registry() {}
    Registry(const Registry&) ;
    void operator=(const Registry&) ;

public:

    static Registry& instance() ;

    void registerFactory(const std::string& type, ComponentFactory factory) ;
    Component* createComponent(const std::string& type) ;

private:

    CompId _nextId ;
    FactoryMap _factories ; 
    ComponentMap _components ;

} ;


}} ;
