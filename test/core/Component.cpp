#include <theoria/core/Component.h>
#include <gtest/gtest.h>

namespace theoria { 

class CalcInterface 
{
public:

    virtual ~CalcInterface() ;

    virtual int calc(int x, int y) const = 0 ;
} ;

class ComponentImplCalcInterface : public CalcInterface, public core::Component
{
public:

    int calc(int x, int y) const override ;
} ;

int ComponentImplCalcInterface::calc(int x, int y)
{
    return x * y ;
}

class ComponentOwnsCalcInterface public core::Component
{

    class CalcInterfaceImpl : public CalcInterface
    {
    public:

        int calc(int x, int y) const override ;
    } ;

public:

    ComponentOwnsCalcInterface() ;
    
    core::Component* bind(const type_info& type, void ** dest) override ;

private:

    CalcInterface* p_impl ;
    
} ;

ComponentOwnsCalcInterface::ComponentOwnsCalcInterface()
{
    p_impl = new ComponentOwnsCalcInterface::CalcInterfaceImpl() ;
}



core::Component* bind(const type_info& type, void ** dest) override 
{
    *dest = nullptr ;
    if (type == typeid(CalcInterface)) {
        *dest = p_impl ;
    }    
    return nullptr; 
}

class ComponentKnowsCalcInterfaceImpl : public core::Component
{
public:

    core::Component* bind(const type_info& type, void ** dest) override ;

} ;

class ComponentKnowsCalcInterfaceOwn : public core::Component
{
public:

    core::Component* bind(const type_info& type, void ** dest) override ;
} ;

}
