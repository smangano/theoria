#pragma once

#include <theoria/core/primitives.h>
#include <theoria/core/Dependencies.h>
#include <theoria/util/densemap.h>
#include <theoria/util/Maybe.h>

#include <string>
#include <map>
#include <memory>

namespace theoria { 

/*
 * Theoria Forward Declarations
 */
namespace config {
    
    class Config ;

} //namespace config


namespace core {

/*
 * Forward Declarations
 */
class Component ;
class Theoria ;

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

    ~Registry() ;

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
     * @factory the factory function
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
     * @allow_ambiguity if false treat case (3) above as ambiguous and raise an exception
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

    /* Create a component that could satisfy a dependency, if possible
     *
     * @dep the dependency that needs to be satisfied
     * @return the component or nullptr if could not satisfy or otherwise failed to create 
     */
    Component* createComponent(const Dependencies::Dependent& dep) noexcept ;

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
     * @predicate test criteria
     * @return first entry at or after start that satisfies predicate or endFact()
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

    /*
     * If you know a component's id then this is the fastest and safest way to
     * access the component
     *
     */
    util::Maybe<Component> component(CompId id) 
    {
        try {
            return util::Maybe<Component>(_components.at(id)) ;
        }
        catch (...) {
            return util::Maybe<Component>(nullptr) ;
        }
    }

    /* Return component with the specified type. If multiple subtypes exist it will return
     * the first one it finds
     *
     * @type the type name of the component
     * @return Option evaluates to true if found, false otherwise.
     *         Derfef *option returns a reference to component or raises exception
     *         See Option.h for more behavior
     */
    util::Maybe<Component> component(const TypeName& type) ;
    util::Maybe<Component> component(const TypeName& type, const SubTypeName& subtype) ;
 
    /* Get a component that is suitable for satisfying dep
     *
     * @dep a dependency
     */
    util::Maybe<Component> component(const Dependencies::Dependent& dep) ;

    template <typename T>
    util::Maybe<T> componentAs(CompId id) {
        return util::Maybe<T>(component(id).get()) ;
    }

    template <typename T>
    util::Maybe<T> componentAs(const TypeName& type) {
        return util::Maybe<T>(component(type).get()) ;
    }

    template <typename T>
    util::Maybe<T> componentAs(const TypeName& type, const SubTypeName& subtype) {
        return util::Maybe<T>(component(type, subtype).get()) ;
    }


    const config::Config& bootConfig() const ; 
    const config::Config& appConfig() const ; 
  
    /* Satisfy dependencies as required by deps, creating components if necessary
     *
     * @deps  the dependencies to satisfy
     * @compId optional compId of requesting component. Used for informational purposes
     * @return the vector components which satisfy (possibly nullptr if could not satisfy)
     *
     */
    std::vector<Component*> satisfy(const Dependencies& deps, CompId compId=-1) ;

    /*
     * At the moment does nothing but reserved to allow resources
     * to be freed when no longer needed
     */
    void release(Component* component) {};

    /*
     * At the moment does nothing but reserved to allow dynamic
     * dependency tracking in cases where dependencies are not
     * fully known after initialization. For example, 
     * When builder components wire things up on their own
     */
    void addDep(Component* from, Component* to)  {} 

private:
    
    friend class Theoria ;
    

    void _setBootstrapConfig(std::unique_ptr<config::Config>& pBSConfig) ; 
    void _setAppConfig(std::unique_ptr<config::Config>& pAppConfig) ;

    Component* _createComponent(FactoryMap_iterator iter) ;

    CompId _nextId ;
    FactoryMap _factories ; 
    ComponentMap _components ;
    ComponentXrefMap _xref ;
    const config::Config * _bootstrapConfig ;
    const config::Config * _appConfig ;
} ;


}} //namespace theoria::core
