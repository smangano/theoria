#pragma once

#include <theoria/core/primitives.h>
#include <theoria/core/Dependencies.h>
#include <theoria/util/densemap.h>
#include <theoria/util/Maybe.h>

#include <chrono>
#include <map>
#include <memory>
#include <string>

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

class RegistryLocked : public std::exception
{
} ;

/**
 * Used to lock the Registry for iteration in a multi-threaded env
 */
struct RegistryLock
{
    /**
     * Lock registry or block if already locked
     */
    RegistryLock() ;

    /**
     * Try to lock registry ntimes with sleep ms between. Raises exception if lock not aquired
     */
    RegistryLock(int ntimes, long sleepms = 0.0) ;

    /**
     * Try to lock registry ntimes with sleep sleepduration between. Raises exception if lock not aquired
     */
    template< class Rep, class Period >
    RegistryLock(int ntimes, std::chrono::duration<Rep, Period> sleepduration) ;
    
    /**
     * Destructor releases lock
     */
    ~RegistryLock() ;

    /**
     * Return true if not locked. Not particularly useful outside of unit test
     */
    static bool testLock() ;
};


/**
 * The Registry manages Theoria Compoents and their factories. It is the central mechanism that enables dependency injection 
 * as well as on demand access to components.
 *
 * The registry is the only Singleton within Theoria and you can leverage it to avoid creating Singletons within your own application.
 */
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

    /**
     * Destructor
     */
    ~Registry() ;

    /**
     * Type of iterator over component factories
     */
    using FactoryMap_iterator = FactoryMap::iterator;

    /**
     * Singleton accessor
     */
    static Registry& instance() ;

    
    /** 
     * Register the default factory for components of type
     * @param type the type name
     * @param factory the function for creating components
     */
    void registerFactory(const TypeName& type, ComponentFactory factory) ;

    /**
     * Register the factory for components of type and subtype
     * @param type the type name
     * @param subtype the subtype name
     * @param factory the factory function
     */
    void registerFactory(const TypeName& type, const SubTypeName& subtype, ComponentFactory factory) ;

    /**
     * Unregister the factory. Components of this type can no longer be created
     * @param type the type name
     */
    void unregisterFactory(const TypeName& type) ;

    /**
     * Unregister the factory. Components of this type and subtype can no longer be created
     * @param type the type name
     * @param subtype the subtype name
     */
    void unregisterFactory(const TypeName& type, const SubTypeName& subtype) ;

    /**
     * Create a component of the specified type. The following type resolution rules are used:
     * 1) If there is only one factory for the specified type, use that.
     * 2) If there are multiple factories and none has been used then use the default factory whose subtype==type
     * 3) If there are multiple factories and none has been used and there is no default, use the first one (unless
     *    allow_ambiguity is false)
     * 4) If there are multiple factories use the first one that has already been used
     *
     * @param type the type name to use for look up
     * @param allow_ambiguity if false treat case (3) above as ambiguous and raise an exception
     *
     * @throw std::runtime_error if type not found
     */
    Component* createComponent(const TypeName& type, int allow_ambiguity = true) ;

    /**
     * Create a component of the specified type and subtype. 
     *
     * @param type the type name to use for look up (primary key)
     * @param subtype the sub type name to use for look up (secondary key)
     *
     * @throw std::runtime_error if factory for both type and subtype does not exist
     */
    Component* createComponent(const TypeName& type, const SubTypeName& subtype) ;

    /**
     * Create a component that could satisfy a dependency, if possible
     *
     * @param dep the dependency that needs to be satisfied
     * @return the component or nullptr if could not satisfy or otherwise failed to create 
     */
    Component* createComponent(const Dependencies::Dependent& dep) noexcept ;

    /**
     * Begin iterator over factories.
     *
     * Thread Safety: Requires RegistryLock()
     */
    FactoryMap_iterator beginFact() ;

    /**
     * End iterator over factories.
     *
     * Thread Safety: Requires RegistryLock()
     */
    FactoryMap_iterator endFact() ;

    /**
     * Find factory by type.
     * @param type the type name to find
     * @return first entry for type or endFact()
     *
     * Thread Safety: Requires RegistryLock()
     */
    FactoryMap_iterator findFact(const TypeName& type) ;

    /**
     * Find factory by predicate.
     * @param start where to start searching
     * @param predicate test criteria
     * @return first entry at or after start that satisfies predicate or endFact()
     *
     * Thread Safety: Requires RegistryLock()
     */
    FactoryMap_iterator findfact(FactoryMap::iterator start, bool (*predicate)(FactoryMap::value_type& v))  ;

    /**
     * Iterator to first component
     */
    ComponentMap::iterator beginComp() ;

    /**
     * Iterator to _past-the-end_ component
     */
    ComponentMap::iterator endComp() ;

    /**
     * Iterator o first component (as const)
     */
    ComponentMap::const_iterator beginComp() const ;

    /**
     * Iterator to _past-the-end_ component (as const)
     */
    ComponentMap::const_iterator endComp() const ;

    /**
     * Dump the registry
     * @param stream destionation of dump
     */
    void dump(std::ostream& stream) const ;

    /**
     * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     * Very dangerous. Only valid use-case is unit-testing.
     * Wipes the entire state of the Registry
     * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     */
    void reset() ;

    /**
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

    /**
     * Return component with the specified type. If multiple subtypes exist it will return
     * the first one it finds
     *
     * @param type the type name of the component
     * @return Maybe evaluates to true if found, false otherwise.
     *         Derfef *Maybe returns a reference to component or raises exception
     *         @see <theoria/util/Maybe.h> for more behavior
     */
    util::Maybe<Component> component(const TypeName& type) ;


    /**
     * Return component with the specified type and subtype. 
     * 
     *
     * @param type the type name of the component
     * @param subtype the subtype of the component
     * @return Maybe evaluates to true if found, false otherwise.
     *         Derfef *Maybe returns a reference to component or raises exception
     *         @see <theoria/util/Maybe.h> for more behavior
     */
    util::Maybe<Component> component(const TypeName& type, const SubTypeName& subtype) ;
 
    /**
     * Get a component that is suitable for satisfying dep
     *
     * @param dep a dependency
     * @return a Maybe<T> which is bound if the operation was successful
     */
    util::Maybe<Component> component(const Dependencies::Dependent& dep) ;

    /**
     * Get component as T via dynamic cast
     * @param id the id of the component
     * @return a Maybe<T> which is bound if the operation was successful
     */
    template <typename T>
    util::Maybe<T> componentAs(CompId id) {
        return util::Maybe<T>(component(id).get()) ;
    }

    /**
     * Get component as T via dynamic cast
     * @param type the type of the component
     * @return a Maybe<T> which is bound if the operation was successful
     */
    template <typename T>
    util::Maybe<T> componentAs(const TypeName& type) {
        return util::Maybe<T>(component(type).get()) ;
    }

    /**
     * Get component as T via dynamic cast
     * @param type the type of the component
     * @param subtype of the compoent
     * @return a Maybe<T> which is bound if the operation was successful
     */
    template <typename T>
    util::Maybe<T> componentAs(const TypeName& type, const SubTypeName& subtype) {
        return util::Maybe<T>(component(type, subtype).get()) ;
    }

    /**
     * Access to theoria bootstrap config
     */
    const config::Config& bootConfig() const ; 

    /**
     * Access to the application config
     */
    const config::Config& appConfig() const ; 
  
    /**
     * Satisfy dependencies as required by deps, creating components if necessary
     *
     * @param deps  the dependencies to satisfy
     * @param compId optional compId of requesting component. Used for informational purposes
     * @return the vector components which satisfy (possibly nullptr if could not satisfy)
     */
    std::vector<Component*> satisfy(const Dependencies& deps, CompId compId=-1) ;

    /**
     * At the moment does nothing but reserved to allow resources
     * to be freed when no longer needed
     */
    void release(Component* component) {};

    /**
     * At the moment does nothing but reserved to allow dynamic
     * dependency tracking in cases where dependencies are not
     * fully known after initialization. For example, 
     * When builder components wire things up on their own
     */
    void addDep(Component* from, Component* to)  {} 

private:
    
    friend class Theoria ;
    

    void _setBootstrapConfig(std::unique_ptr<const config::Config>& pBSConfig) ; 
    void _setAppConfig(std::unique_ptr<const config::Config>& pAppConfig) ;

    Component* _createComponent(FactoryMap_iterator iter) ;

    CompId _nextId ;
    FactoryMap _factories ; 
    ComponentMap _components ;
    ComponentXrefMap _xref ;
    std::unique_ptr<const config::Config> _bootstrapConfig ;
    std::unique_ptr<const config::Config> _appConfig ;
} ;


}} //namespace theoria::core
