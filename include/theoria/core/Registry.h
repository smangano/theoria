#include <theoria/core/primitives.h>
#include <theoria/util/densemap.h>

#include <string>

#include <unordered_map>

namespace theoria { namespace core {

class Component ;


class Registry
{
private:

    using FactoryMap = std::map<CompFactoryKey, ComponentFactory> ;
    using ComponentMap = util::densemap<CompId, Component*> ;

    Registry() {}
    Registry(const Registry&) ;
    void operator=(const Registry&) ;

public:

    static Registry& instance() ;
    
    /* Register the default factory for components of type
     * @type the type name
     * @factory the function for creating components
     */
    void registerFactory(const TypeName& type, ComponentFactory factory) ;

    /*
     * Register the factory for components of type and subtype
     * @type the type name
     * @subtype the subtype name
     * @factory the factory fuction
     */
    void registerFactory(const TypeName& type, const SubTypeName& subtype, ComponentFactory factory) ;

    /*
     * Create a component of the specified type. The following type resolution rules are used:
     * 1) If there is only one factory for the specified type, use that.
     * 2) If there are multiple factories and none has been used then use the default factory whose subtype==type
     * 3) If there are multiple factories and none has been used and there is no default, use the first one
     * 4) If there are multiple factories use the first one that has already been used
     */
    Component* createComponent(const TypeName& type) ;
    Component* createComponent(const TypeName& type, const SubTypeName& subtype) ;

private:

    CompId _nextId ;
    FactoryMap _factories ; 
    ComponentMap _components ;

} ;


}} ;
