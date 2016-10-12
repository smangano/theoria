#include "theoria/core/Registry.h"
#include "theoria/core/Component.h"
#include <gtest/gtest.h>

#include <ostream>

using namespace theoria ;
using namespace core ;

class RegistryTest : public ::testing::Test 
{
    virtual void TearDown() {
        Registry::instance().reset() ;
    } 
} ;

struct MockComponent : public Component
{
    MockComponent(CompId id):
        Component(id, "MockCompoent") {}

    static Component* factory(CompId id) {
        return new MockComponent(id) ;
    }


} ;

struct MockComponent2 : public MockComponent
{
    MockComponent2(CompId id):
        MockComponent(id) {_name = "MockComponent2";}

    static Component* factory(CompId id) {
        return new MockComponent2(id) ;
    }

} ;

TEST_F(RegistryTest, RegisterFactoryByTypenameOnly) 
{
    Registry::instance().registerFactory("MockComponent", MockComponent::factory) ; 

    ASSERT_NE(Registry::instance().beginFact(), Registry::instance().endFact()) ;
    ASSERT_NE(Registry::instance().findFact("MockComponent"), Registry::instance().endFact()) ;
    ASSERT_EQ(Registry::instance().findFact("MockComponent")->first, 
              std::make_pair(std::string("MockComponent"), std::string("MockComponent"))) ;
}

TEST_F(RegistryTest, RegisterFactoryByTypenameAndSubType) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent", MockComponent::factory) ; 
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 

    ASSERT_NE(Registry::instance().beginFact(), Registry::instance().endFact()) ;
    ASSERT_NE(Registry::instance().findFact("MockComponent"), Registry::instance().endFact()) ;
    auto iter = Registry::instance().findFact("MockComponent") ;
    ASSERT_EQ(iter->first, 
              std::make_pair(std::string("MockComponent"), std::string("MockComponent"))) ;
    ASSERT_EQ(iter->second, MockComponent::factory) ;
    ++iter ;
    ASSERT_EQ(iter->first, 
              std::make_pair(std::string("MockComponent"), std::string("MockComponent2"))) ;
    ASSERT_EQ(iter->second, MockComponent2::factory) ;
}

TEST_F(RegistryTest, CreateByTypenameOnlyOneRegister) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 
    Component *comp =  Registry::instance().createComponent("MockComponent") ;

    ASSERT_NE(comp, nullptr) ;
    MockComponent2* mock2 = dynamic_cast<MockComponent2*>(comp) ;
    ASSERT_NE(mock2,nullptr) ;
}

TEST_F(RegistryTest, CreateByTypenameTwoRegisterUseDefault) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 
    Registry::instance().registerFactory("MockComponent", MockComponent::factory) ; 
    Component *comp =  Registry::instance().createComponent("MockComponent") ;

    ASSERT_NE(comp, nullptr) ;
    MockComponent* mock = dynamic_cast<MockComponent*>(comp) ;
    ASSERT_NE(mock,nullptr) ;
    MockComponent2* mock2 = dynamic_cast<MockComponent2*>(comp) ;
    ASSERT_EQ(mock2,nullptr) ;
}

TEST_F(RegistryTest, CreateByTypenameTwoRegisterUseAlreadyUsed) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 
    Registry::instance().registerFactory("MockComponent", MockComponent::factory) ; 

    //Use Mock2
    Component *comp =  Registry::instance().createComponent("MockComponent", "MockComponent2") ;
    ASSERT_NE(comp, nullptr) ;
    MockComponent2* mock2 = dynamic_cast<MockComponent2*>(comp) ;
    ASSERT_NE(mock2,nullptr) ;

    //Now this returns Mock2 becaused used one preferred
    comp =  Registry::instance().createComponent("MockComponent") ;
    ASSERT_NE(comp, nullptr) ;
    mock2 = dynamic_cast<MockComponent2*>(comp) ;
    ASSERT_NE(mock2,nullptr) ;
}

TEST_F(RegistryTest, CreateByTypenameTwoRegisterUseFirst) 
{
    Registry::instance().registerFactory("MockComponent", "M1", MockComponent2::factory) ; 
    Registry::instance().registerFactory("MockComponent", "M2", MockComponent::factory) ; 

    //Now this returns Mock2 becaused it is first
    Component *comp =  Registry::instance().createComponent("MockComponent") ;
    ASSERT_NE(comp, nullptr) ;
    MockComponent2* mock2 = dynamic_cast<MockComponent2*>(comp) ;
    ASSERT_NE(mock2,nullptr) ;
}

TEST_F(RegistryTest, CreateByTypenameTwoRegisterErrorDueToNoAmbiguity) 
{
    Registry::instance().registerFactory("MockComponent", "M1", MockComponent2::factory) ; 
    Registry::instance().registerFactory("MockComponent", "M2", MockComponent::factory) ; 

    //Now this returns Mock2 becaused it is first
    bool allow_ambiguity = false ;
    ASSERT_THROW(Registry::instance().createComponent("MockComponent", allow_ambiguity), std::runtime_error) ;
}
