#pragma once

#include <theoria/core/primitives.h>
#include <theoria/core/Dependencies.h>
#include <theoria/except/except.h>

#include <string>
#include <typeinfo>

namespace theoria { namespace config { class Config ; }}

namespace theoria { namespace core {

/*
 * Data associated with a component 
 */
class ComponentData
{
    using BaseList = std::vector<CompId> ;

public:

    using baseid_const_iterator = BaseList::const_iterator ;

    /*
     * Componets can be linked to base implementations to support
     * the Theoria's concept of _dynamic inheritence_.
     */
    baseid_const_iterator beginBaseId() const ;
    baseid_const_iterator endBaseId() const ;

    /*
     * Component's id
     */
    CompId id() const {return  _id; }

    /*
     * Component's Type name
     */
    TypeName type() const {return _type;}

    /*
     * List of component's immediate base type names
     */
    TypeNameList baseTypes() const ;

    /*
     * List of all components ancestor types aquired using breadth first traversal with dupes
     * eliminated
     */
    TypeNameList ancestorTypes() const ;

    /*
     * Component's description. May contain markdown (http://daringfireball.net/projects/markdown/).
     */
    Text desc() const {return _desc ;} 

    
private:

	CompId _id ;
	BaseList _bases ;
	std::string _type ;
	std::string _name ;
	Text _desc ;
} ;

/*
class: CompState
desc:  A component is always in some state or composition of states. CompState models a non-empty
       state list where the primary state is on the the first and secondary states are later in the list. 
**/

//        STATE                                | Value     | Typical Usage
//--------------------------------------------------------------------------------------------------------
const int OKAY_STATE 						    =  0 ; //  | Everything is Okay
const int WARN_STATE 							=  1 ; //  | Possible issue. Not critical
const int ERROR_STATE 							=  2 ; //  | Component enccountered an error. Probably serious
const int DISABLED_STATE 						=  3 ; //  | The component has been disabled
const int CONNECTED_STATE 						=  4 ; //  | The component is connected to some external resource
const int DISCONNECTED_STATE 					=  5 ; //  | The component is disconnected from an external resource
const int STARTED_STATE 						=  6 ; //  | The component has started processing (may be used in assoc with threading)
const int STOPPED_STATE 						=  7 ; //  | The component has stopped processing (may be used in assoc with threading)
const int INIT_STATE 							=  8 ; //  | The component ititialized successfully
const int WAITING_STATE 						=  9 ; //  | The component is waiting on something
const int READY_STATE 							= 10 ; //  | The component is ready to perform its intended function
const int EXCEPTION_STATE   					= 11 ; //  | The component raised an exception

const int USER_STATE_START                      = 1000 ;

/**
 * class CompState - component state managment. State management is an intergral aspect of component visualization
 */
class CompState
{

	/**
     * Set primary state
	 */
	void set(int state) ;

	/**
	 * Clear state vector and set primary state to state (OKAY by default)
	 */
	void clear(int state = OKAY_STATE) ;

	/**
	 * Push state making it primary 
	 */
	void push(int state) ;

	/**
	 * Pop and discard primary state. Then only if state vector is empty set
     * primary to 'state' (default OKAY) otherwise ignore 'state'
     */
     void pop(int state = OKAY_STATE) ;

	 /**
	  * Get the 'index'th state. Default primary
      */
	 int get(int index=0)  const ;

	 /**
      * Test that primary state is OKAY
	  */
	 bool isOKAY() const ;

	 /**
	  * Return true if one of the states is 'state'
	  */
	 bool has(int state) const ;


	 /**
	  * Return index of 'state' or -1 if no such state exists
      */
	 int find(int state) const ;

	 /**
      * Return true if any state in 'states' exists
      */
     bool hasOne(int states[]) const ;

	 /**
      * Return true if all states in 'states' exist
      */
     bool hasAll(int states[]) const ;

	 /**
      * Return true if no state in 'states' exists
      */
	 bool hasNone(int states[]) const ;

private:

	std::vector<int> _states ;

};


union MsgData
{
	const std::pair<int,int> d2 ;
    const long long          dl ;
    const void *             dp ;
} ;

class Message
{
private:

	CompId origSenderId ;
    CompId origDestId ;
	CompId lastSenderId ;
    int    msgType ;
    MsgData msgData ;  
} ; 

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
     * Recieve the componets you requried in init(). Optional components will be nullptrs.
     * All components received are itialized but not necessarily finalized so do not call 
     * into the component yet (unless you really know what you are doing but you are prob asking for trouble)
     */
    virtual void finalize(std::vector<Component*>& dependencies) ;

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
	void onMessage(Message msg) ;

    CompId id() const {return _id;}

    const std::string& name() const {return _name;}
    void setName(const std::string& name) {_name = name;}

    template <class T>
    T* cast(const std::string& requestor = "Unknown") 
    {
        const std::type_info& ti = typeid(T) ;
        Component * target = this ;
        //To avoid posibility of infinit loops we cap this protocol at two attemps
        for (int iTry=0; iTry<2 && target; ++iTry) { 
            //First see if the Component is a T
            T* pT = dynamic_cast<T*>(target) ;
            if (pT) //Okay I'm a T!
                return pT ;
            //Otherwise maybe I own a T. My implentor is then responsible for providing
            target = acquire(ti, reinterpret_cast<void **>(&pT)) ;
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
