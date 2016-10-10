#pragma once

#include <core/primitives.h>

#include <string>

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
     */*
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
     void pop(state = OKAY_STATE) ;

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
	 int find(state) const ;

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

/**
 * class Dependents
 * -----------------
 * Used by components during initialization to advertise there requirements for other components. 
 * Dependencies can be loose if they only require they conform to some type or tight if they name a specific implementation
 * Dependicies can be optional if the component can ontinue (possibly in a reduced functional state) without the dependency being
 * satisfied. 
 * 
 * Usage Examples:
 *
 *       return Dependencies().loose("Type1").loose("Type2").default("TheFoo").strict("Type3", "ASpecificType3") 
 *
 * If you prefer a specific implementation but will settle for a generic one it is perfectly fine to do this:
 *
 *       return  Dependencies().strict("Type1", "Impl1",Dependent::optional).loose("Type1") ;
 * 
 * If you are willing to do all together without any:
 *  
 *       return  Dependencies().strict("Type1", "Impl1",Dependent::optional).loose("Type1",Dependent::optional) ;
 *        
 */
class Dependencies
{
	struct Dependent
	{
		Dependent(const std::string& type_, const std::string& name_, bool optional_ = false):
        	: type(type_), name(name_), optional(optional_) {}

        Dependent(const std::string& type_, optional_=false)
            : type(type_), name(type_), optional(false) 


		std::string type ;
		std::string name ;
		bool optional ;
	} ;

	Dependencies() {} 

	/**
     * Add a dependent to a component of the given type. If there are multiple compnents of this type it will pick the first one that
     * has existing dependencies otherwise the first one it sees.
     */
	Dependencies& loose(const std::string type, bool optional=false) {_deps.push_back(Dependent(type,"",optional)) ; return *this}

	/**
     * Add a dependent to a component of the given name such that the type matches it's name . Such a component can be thaought of as the default component of that
     * type and there can be only 0 or 1. 
     */
    Dependencies& default(const std::string name, bool optional=false) {_deps.push_back(Dependent(name,name, optional)) ; return *this}

	/**
	 * Add a dependent with the specified name and type. There can be only 0 or 1. This is a strict dependency because the dependent is anouncing that one and only
     * one implementation will do
     */
    Dependencies& strict(const std::string& type, const std::string& name, bool optional=false) {_deps.push_back(Dependent(type,name, optional)) ; return *this}

	
private:

	std::vector<Dependent> _deps ;
} ;

enum class AppLifeCycle
{
    INITITIALIZED, /* All Components Initialized. Nothing Finalized  yet.*/
    FINALIZED,     /* All Components Finalized. Feel free to interact */
    PRESHUTDOWN,   /* About to shutdown but all compoents are safe */
    SHUTDOWN,      /* Last chance to perform shutdown actions. Your dependencies may be unsafe */
    FORCE_SHUTDOWN, /* All bets are off. User forced shutdown. Log some final state but don't do much else */
    DUMP,           /* User triggered App Dump. Log as much useful information as you can about yourself. Avoid interaction with others */
} ;

enum class CompLifeCycle
{
    INALIZED, 		/* Dependent component finalized. You may not be */
    DISABLED,  		/* Dependent component was disable */
    ENABLED,   		/* Dependent component was enabled after having been previously disabled */
    EXCEPTION, 		/* Dependent component raised an exception  with broacast */
    STATE_CHANGE, 	/* Dependent component triggered a state change with broadcast */
} ;

union MsgData
{
	const std::pair<int,int> d2 ;
    cosnt long long          dl ;
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

	/**
     * Initialize component from its configuration and return the Components dependencies
     */
	virtual Dependencies init(const Config& config) ;

	/**
     * Recieve the componets you requried in init(). Optional components will be nullptrs.
     * All components received are itialized but not necessarily finalized so do not call 
     * into the component yet (unless you really know what you are doing but you are prob asking for trouble)
     */
    virtual void finalize(std::vector<Component*> dependencies) ;

	/**
     * This is a place to take action on application lifecycle events. 
     */
    void appLifeCycle(AppLifeCycle state) ;

	/**
     * This is a place to take action on events related to components you depend on
     */
    void compLifeCycle(CompLifeCycle state, CompId id) ;


	/**
	 * If your coponent is a member of a event loop, async messages arrive here 
     */ 
	void onMessage(Message msg) ;

	
private:

	std::string _name ;
	CompId _id ;
};

}} //namespace theoria::core	
