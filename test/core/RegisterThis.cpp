#include <theoria/core/Register.h>
#include "theoria/core/Component.h"
#include <gtest/gtest.h>


using namespace theoria ;
using namespace core ;


class RegisterThisTest : public ::testing::Test 
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

TEST_F(RegistryTest, RegisterFactoryByTypenameOnly) 
{
    RegistryThis<MockComponent> rt {"MockComponent"} ;

    ASSERT_NE(Registry::instance().beginFact(), Registry::instance().endFact()) ;
    ASSERT_NE(Registry::instance().findFact("MockComponent"), Registry::instance().endFact()) ;
    ASSERT_EQ(Registry::instance().findFact("MockComponent")->first, 
              std::make_pair(std::string("MockComponent"), std::string("MockComponent"))) ;
}



