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

class CalcInterface2 
{
public:

    virtual ~CalcInterface2() ;

    virtual int calc(int x, int y) const = 0 ;
} ;

CalcInterface2::~CalcInterface2()
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

TEST(TestComponentImplCalcInterface, TestCast) {
   ComponentImplCalcInterface comp ;

   ASSERT_NE(comp.cast<CalcInterface>(), nullptr) ; 
   EXPECT_EQ(comp.cast<CalcInterface>()->calc(2,2), 4) ;
   EXPECT_THROW(comp.cast<CalcInterface2>(), std::runtime_error) ;
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
    
    core::Component* acquire(const std::type_info& type, void ** dest) override ;

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



core::Component* ComponentOwnsCalcInterface::acquire(const std::type_info& type, void ** dest)  
{
    *dest = nullptr ;
    if (type == typeid(CalcInterface)) {
        *dest = p_impl ;
    }    
    return nullptr; 
}

TEST(TestComponentOwnsCalcInterface, TestCast) {
   ComponentOwnsCalcInterface comp ;

   ASSERT_NE(comp.cast<CalcInterface>(), nullptr) ; 
   EXPECT_EQ(comp.cast<CalcInterface>()->calc(3,2), 5) ;
   EXPECT_THROW(comp.cast<CalcInterface2>(), std::runtime_error) ;
}

/////////////////////
class ComponentKnowsCalcInterfaceImpl : public core::Component
{
public:

    core::Component* acquire(const std::type_info& type, void ** dest) override ;

private:

    ComponentImplCalcInterface _impl ;
} ;

core::Component* ComponentKnowsCalcInterfaceImpl::acquire(const std::type_info& type, void ** dest) 
{
    *dest = nullptr ;
    return &_impl ;
}

/////////////////////
class ComponentKnowsCalcInterfaceOwn : public core::Component
{
public:

    core::Component* acquire(const std::type_info& type, void ** dest) override ;

private:

    ComponentOwnsCalcInterface _impl ;
} ;

core::Component* ComponentKnowsCalcInterfaceOwn::acquire(const std::type_info& type, void ** dest) 
{
    *dest = nullptr ;
    return &_impl ;
}

}
