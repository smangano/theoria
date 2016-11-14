#pragma once

#include <theoria/core/primitives.h>
#include <theoria/core/Registry.h>

namespace theoria { namespace core {

/**
 * Add a static instance of this class to your Component to cause it to register itself
 * automatically. 
 *
 * Example:
 *   
 *  //MyComponent.h
 *  //--------------
 *
 *  class MyComponent : public Component
 *  {
 *  public:
 *
 *      //...
 *
 *  private:
 *
 *      //...
 *
 *      static RegisterThis<MyComponent> _RT ;
 *  };
 *   
 *  //MyComponent.cpp
 *  //---------------
 *
 *  MyComponent::_RT {"MyComponent"} ;
 *  
 */

template <typename TComp>
class RegisterThis
{
public:

    /**
     * Regsiter TComp of specified type
     */
    RegisterThis(const TypeName& type)
    {
        Registry::instance().registerFactory(type, TComp::factory) ;
    }

    /**
     * Regsiter TComp of specified type and subtype
     */
    RegisterThis(const TypeName& type, const SubTypeName& subtype)
    {
        Registry::instance().registerFactory(type, subtype, TComp::factory) ;
    }
 
};

}}
