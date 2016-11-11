#pragma once

#include <theoria/core/primitives.h>
#include <theoria/core/Dependencies.h>
#include <theoria/except/except.h>

#include <string>
#include <typeinfo>

namespace theoria { namespace config { class Config ; }}

namespace theoria { namespace core {

/** 
 * Components are the soul of Theoria. Components are used to implement the 
 * meaty parts of your application. Things that contain significant business logic,
 * are generally long-lived, manage complex data structures, processes or algorithms, etc.
 * Components support a rich lifecycle, can be dynamically configured and dynamically wired up
 * to other components.
 *
 * Components consume config data to customize themselves to a particular use. Config data is provided 
 * in the init() method and it is the first stage of a components lifecycle. Often this 
 * cofiguration will provide information that will tell a component what other components it
 * requires but other times the configuration data will be simple paramters like a host/port
 * or locale info, or user settings, etc.  
 *
 * In response to configuraation a component is expected to return its requirements for other components. It does this
 * by returning Dependencies. Dependencies are a way of conveying very general or very specific requirements that 
 * of this component to other components. A specific requirement is communicated as a Type and Subtype. Here we
 * use Type to mean "the need of a component that provides some service" and Subtype to mean a very specific implematation of that
 * service. In addition, Dependencies convey whether a service is absolutly required or is optional. An absolute requirement must 
 * be fullfiled for the application to progress.
 *
 * Once all components in you application are initialized, Theoria satisfies dependencies by calling finalize() and handing
 * your componet its dependents in the same order as requested. Optional dependents may be null if Theoria could not aquire them
 *
 *
 */
class Component 
{
public:

	Component() ;
	Component(CompId id):
        _id(id) {}

	Component(CompId id, const std::string& name):
        _id(id), _name(name) {}

    virtual ~Component() ;

	/**
     * Initialize component from its configuration and return the Components dependencies
     */
	virtual Dependencies init(const config::Config& config) ;

	/**
     * Receive the components you required in init(). Optional components will be nullptrs.
     * All components received are initialized but not necessarily finalized so do not call 
     * into the component yet (unless you really know what you are doing but you are prob asking for trouble)
     */
    virtual void finalize(const std::vector<Component*>& dependencies) ;

	/**
     * This is a place to take action on application lifecycle events. 
     */
    virtual void appLifeCycle(AppLifeCycle state) ;

	/**
     * This is a place to take action on events related to components you depend on
     */
    virtual void compLifeCycle(CompLifeCycle state, CompId id) ;


	/**
	 * If your coponent is a member of a event loop, async messages arrive here 
     */ 
	virtual void onMessage(Message msg) ;

    CompId id() const {return _id;}

    const std::string& name() const {return _name;}
    void setName(const std::string& name) {_name = name;}

    template <class T>
    T* cast(const std::string& requestor = "Unknown") 
    {
        const std::type_info& ti = typeid(T) ;
        Component * target = this ;
        //To avoid possibility of infinite loops we cap this protocol at two attempts
        for (int iTry=0; iTry<2 && target; ++iTry) { 
            //First see if the Component is a T
            T* pT = dynamic_cast<T*>(target) ;
            if (pT) //Okay I'm a T!
                return pT ;
            //Otherwise maybe I own a T. My implementor is then responsible for providing
            target = target->acquire(ti, reinterpret_cast<void **>(&pT)) ;
            if (pT) //Yay, i'm bound. 
                return pT ;
        }
        throw RUNTIME_ERROR("Can't cast Component [%s] to type [%s] as required by [%s]", name().c_str(), ti.name(), requestor.c_str()) ; 
    }

    /*
     * Override in your component if you want to control how other components acquire interfaces
     * from you. Your are responsible for using the typeInfo to return a suitable object
     * in destination. If you can't do this you can optionally return another Component that
     * you think can.
     *
     * Default impl sets *dest to nullptr and returns nullptr 
     */
    virtual Component* acquire(const std::type_info& typeInfo, void** dest) ;

protected:

	CompId _id ;
	std::string _name ;
};

}} //namespace theoria::core	
