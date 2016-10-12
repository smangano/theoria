#include "theoria/core/Registry.h"
#include "theoria/core/Component.h"
#include <gtest/gtest.h>

using namespace theoria ;
using namespace core ;

class RegistryTest : public ::testing::Test 
{
    virtual void TearDown() {
        Registry::reset() ;
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
}
