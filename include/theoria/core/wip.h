#pragma once

#include <theoria/core/primitives.h>
#include <theoria/core/Dependencies.h>
#include <theoria/except/except.h>

#include <string>

namespace theoria { namespace core {
/**
 * Data associated with a component for documentation purposes 
 */
class ComponentData
{
    using BaseList = std::vector<CompId> ;

public:

    using baseid_const_iterator = BaseList::const_iterator ;

    /*
     * Components can be linked to base implementations to support
     * the Theoria's concept of _dynamic inheritance_.
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
     * List of all components ancestor types acquired using breadth first traversal with dupes
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

/**
CompState

A component is always in some state or composition of states. CompState models a non-empty state list 
where the primary state is on the the first and secondary states are later in the list. 
*/

//        STATE                                | Value     | Typical Usage
//--------------------------------------------------------------------------------------------------------
const int OKAY_STATE 						    =  0 ; //  | Everything is Okay
const int WARN_STATE 							=  1 ; //  | Possible issue. Not critical
const int ERROR_STATE 							=  2 ; //  | Component encountered an error. Probably serious
const int DISABLED_STATE 						=  3 ; //  | The component has been disabled
const int CONNECTED_STATE 						=  4 ; //  | The component is connected to some external resource
const int DISCONNECTED_STATE 					=  5 ; //  | The component is disconnected from an external resource
const int STARTED_STATE 						=  6 ; //  | The component has started processing (may be used in assoc with threading)
const int STOPPED_STATE 						=  7 ; //  | The component has stopped processing (may be used in assoc with threading)
const int INIT_STATE 							=  8 ; //  | The component initialized successfully
const int WAITING_STATE 						=  9 ; //  | The component is waiting on something
const int READY_STATE 							= 10 ; //  | The component is ready to perform its intended function
const int EXCEPTION_STATE   					= 11 ; //  | The component raised an exception

const int USER_STATE_START                      = 1000 ;

/**
 * class CompState - component state management. State management is an integral aspect of component visualization
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


/**
 * Msg payload
 */
union MsgData
{
public:

    MsgData() : dp(nullptr) {} 

	const std::pair<int,int> d2 ;
    const long long          dl ;
    const void *             dp ;
} ;

/**
 * Placeholder for concept that needs much more thought
 */
class Message
{
public:

    Message() :
        origSenderId(-1), origDestId(-1), lastSenderId(-1), msgType(0), msgData() 
    {
    } ;
private:

	CompId origSenderId ;
    CompId origDestId ;
	CompId lastSenderId ;
    int    msgType ;
    MsgData msgData ;  
} ; 

}}
