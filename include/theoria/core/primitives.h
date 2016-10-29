#pragma once

#include <string>
#include <vector>

namespace theoria { namespace core {

class Component ;
using ComponentList = std::vector<Component*> ;
using CompId = int ;
using ComponentFactory = Component* (*)(CompId id) ;
using Text = std::string ;
using TypeName = std::string ; 
using SubTypeName = std::string ;
using CompFactoryKey = std::pair<TypeName, SubTypeName> ;
using TypeNameList = std::vector<TypeName> ;

enum class AppLifeCycle
{
    NONE,
    INITIALIZED, /* All Components Initialized. Nothing Finalized  yet.*/
    FINALIZED,     /* All Components Finalized. Feel free to interact */
    PRESHUTDOWN,   /* About to shutdown but all components are safe */
    SHUTDOWN,      /* Last chance to perform shutdown actions. Your dependencies may be unsafe */
    FORCE_SHUTDOWN, /* All bets are off. User forced shutdown. Log some final state but don't do much else */
    DUMP,           /* User triggered App Dump. Log as much useful information as you can about yourself. Avoid interaction with others */
} ;

enum class CompLifeCycle
{
    DISABLED,  		/* Dependent component was disable */
    ENABLED,   		/* Dependent component was enabled after having been previously disabled */
    EXCEPTION, 		/* Dependent component raised an exception  with broadcast */
    STATE_CHANGE, 	/* Dependent component triggered a state change with broadcast */
} ;

}} //namespace theoria::core

