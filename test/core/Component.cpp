#include <theoria/core/Component.h>
#include <gtest/gtest.h>
#include <typeinfo>

namespace theoria { 

class CalcInterface 
{
public:

    virtual ~CalcInterface() ;

    virtual int calc(int x, int y) const = 0 ;
} ;

CalcInterface::~CalcInterface()
{
}

/////////////////////
class ComponentImplCalcInterface : public CalcInterface, public core::Component
{
public:

    int calc(int x, int y) const override ;
} ;

int ComponentImplCalcInterface::calc(int x, int y) const
{
    return x * y ;
}

/////////////////////
class ComponentOwnsCalcInterface : public core::Component
{

    class CalcInterfaceImpl : public CalcInterface
    {
    public:

        int calc(int x, int y) const override ;
    } ;

public:

    ComponentOwnsCalcInterface() ;
    
    core::Component* bind(const std::type_info& type, void ** dest) override ;

private:

    CalcInterface* p_impl ;
    
} ;

int ComponentOwnsCalcInterface::CalcInterfaceImpl::calc(int x, int y) const 
{
    return x+y ;
}

ComponentOwnsCalcInterface::ComponentOwnsCalcInterface()
{
    p_impl = new ComponentOwnsCalcInterface::CalcInterfaceImpl() ;
}



core::Component* ComponentOwnsCalcInterface::bind(const std::type_info& type, void ** dest)  
{
    *dest = nullptr ;
    if (type == typeid(CalcInterface)) {
        *dest = p_impl ;
    }    
    return nullptr; 
}

/////////////////////
class ComponentKnowsCalcInterfaceImpl : public core::Component
{
public:

    core::Component* bind(const std::type_info& type, void ** dest) override ;

private:

    ComponentImplCalcInterface _impl ;
} ;

core::Component* ComponentKnowsCalcInterfaceImpl::bind(const std::type_info& type, void ** dest) 
{
    *dest = nullptr ;
    return &_impl ;
}

/////////////////////
class ComponentKnowsCalcInterfaceOwn : public core::Component
{
public:

    core::Component* bind(const std::type_info& type, void ** dest) override ;

private:

    ComponentOwnsCalcInterface _impl ;
} ;

core::Component* ComponentKnowsCalcInterfaceOwn::bind(const std::type_info& type, void ** dest) 
{
    *dest = nullptr ;
    return &_impl ;
}

}
