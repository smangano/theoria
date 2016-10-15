@pragma once

#include <theoria/core/primitives.h>

namespace theoria { namespace core

/* Add a static instance of this class to your Component to cause it to register itself
   automatically. 

    Example:
    
    //MyComponent.h
    //--------------

    class MyComponent : public Component
    {
    public:

        //...

    private:

        //...

        static RegisterThis<MyComponent> _RT ;
    };
    
    //MyComponent.cpp
    //---------------

    MyComponent::_RT {"MyComponent"} ;
    
*/

template <typename TComp>
class RegisterThis
{

    RegisterThis(const TypeName& type, int allow_ambiguity=true)
    {
        Registry::instance().register(type, allow_ambiguity, T::factory) ;
    }

    RegisterThis(const TypeName& type, const SubTypeName& subtype)
    {
        Registry::instance().register(type, subtype, T::factory) ;
    }
 
};

}}
