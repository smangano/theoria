#pragma once

#include <theoria/core/primitives.h>
#include <theoria/util/densemap.h>

#include <string>
#include <map>

namespace theoria { namespace core {

class Component ;

/**
 * Used to lock the Registry for iteration in a multi-threaded env
 */
struct RegistryLock
{
    RegistryLock() ;
    ~RegistryLock() ;
};

class Registry
{
private:

    using FactoryMap = std::map<CompFactoryKey, ComponentFactory> ;
    using ComponentMap = util::densemap<CompId, Component*> ;
    using ComponentXrefMap = std::multimap<CompFactoryKey, CompId> ;

    Registry() {}
    Registry(const Registry&) ;
    void operator=(const Registry&) ;

public:


    using FactoryMap_iterator = FactoryMap::iterator;

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
     * 3) If there are multiple factories and none has been used and there is no default, use the first one (unless
     *    allow_ambiguity is false)
     * 4) If there are multiple factories use the first one that has already been used
     *
     * @type the type name to use for look up
     * @allow_ambiguity if false treat case (3) above as abiguous and raise an exception
     *
     * @execept std::runtime_error if type not found
     */
    Component* createComponent(const TypeName& type, int allow_ambiguity = true) ;

    /*
     * Create a component of the specified type and subtype. 
     *
     * @type the type name to use for look up (primary key)
     * @subtype the sub type name to use for look up (secondary key)
     *
     * @throw std::runtime_error if factory for both type and subtype does not exist
     */
    Component* createComponent(const TypeName& type, const SubTypeName& subtype) ;

    /* Begin iterator over factories.
     *
     * Thread Safety: Requires RegistryLock()
     */
    FactoryMap_iterator beginFact() ;

    /* End iterator over factories.
     *
     * Thread Safety: Requires RegistryLock()
     */
    FactoryMap_iterator endFact() ;

    /* Find factory by type.
     * @type the type name to find
     * @return first entry for type or endFact()
     *
     * Thread Safety: Requires RegistryLock()
     */
    FactoryMap_iterator findFact(const TypeName& type) ;

    /* Find factory by predicate.
     * @start where to start searching
     * @predicate test critera
     * @return first entry aot or after start that satisfies predicate or endFact()
     *
     * Thread Safety: Requires RegistryLock()
     */
    FactoryMap_iterator findfact(FactoryMap::iterator start, bool (*predicate)(FactoryMap::value_type& v))  ;

    ComponentMap::iterator beginComp() ;
    ComponentMap::iterator endComp() ;

    ComponentMap::const_iterator beginComp() const ;
    ComponentMap::const_iterator endComp() const ;

    void dump(std::ostream& stream) const ;

    /*
     * Very dangerous. Only know use-case is unit-testing.
     * Wipes the entire state of the Registry
     */
    void reset() ;

    
public: //TODO: private

    Component* _createComponent(FactoryMap_iterator iter) ;

    CompId _nextId ;
    FactoryMap _factories ; 
    ComponentMap _components ;
    ComponentXrefMap _xref ;
} ;


}} 
