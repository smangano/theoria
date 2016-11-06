#include "theoria/core/Registry.h"
#include "theoria/core/Component.h"
#include "theoria/core/RegisterThis.h"
#include "theoria/config/Config.h"
#include <gtest/gtest.h>

#include <ostream>

using namespace theoria ;
using namespace core ;

#include "fixtures/core/mockComponents.cpp"

class RegistryTest : public ::testing::Test 
{
    virtual void TearDown() {
        Registry::instance().reset() ;
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
    Registry& reg = Registry::instance() ;
    reg.registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 
    reg.registerFactory("MockComponent", MockComponent::factory) ; 
    Component *comp =  reg.createComponent("MockComponent") ;

    ASSERT_NE(comp, nullptr) ;
    MockComponent* mock = dynamic_cast<MockComponent*>(comp) ;
    ASSERT_NE(mock,nullptr) ;
    MockComponent2* mock2 = dynamic_cast<MockComponent2*>(comp) ;
    ASSERT_EQ(mock2,nullptr) ;

    auto iter = reg.findfact(reg.beginFact(), [] (auto fact) {return fact.first.second == "MockComponent2";}) ;
    ASSERT_NE(iter, reg.endFact()) ;
    EXPECT_EQ(iter->first.first, "MockComponent") ;
    EXPECT_EQ(iter->first.second, "MockComponent2") ;
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

TEST_F(RegistryTest, CreateByDepNoMatching) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 

    Dependencies::Dependent strict("MockComponent", "MockComponent3") ;
    Component *comp =  Registry::instance().createComponent(strict) ;
    EXPECT_EQ(comp, nullptr) ;

    Dependencies::Dependent defaulted("MockComponent3") ;
    comp =  Registry::instance().createComponent(defaulted) ;
    EXPECT_EQ(comp, nullptr) ;

    Dependencies::Dependent loose("MockComponent3", "") ;
    comp =  Registry::instance().createComponent(loose) ;
    EXPECT_EQ(comp, nullptr) ;
}

TEST_F(RegistryTest, CreateByDepOnlyOneRegister) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 

    Dependencies::Dependent strict("MockComponent", "MockComponent2") ;
    Component *comp =  Registry::instance().createComponent(strict) ;
    ASSERT_NE(comp, nullptr) ;
    EXPECT_EQ(comp->name(), "MockComponent:MockComponent2") ;

    Dependencies::Dependent defaulted("MockComponent") ;
    comp =  Registry::instance().createComponent(defaulted) ;
    EXPECT_EQ(comp, nullptr) ;

    Dependencies::Dependent loose("MockComponent", "") ;
    comp =  Registry::instance().createComponent(loose) ;
    ASSERT_NE(comp, nullptr) ;
    EXPECT_EQ(comp->name(), "MockComponent:MockComponent2") ;
}

TEST_F(RegistryTest, CreateByDepTwoRegistered) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 
    Registry::instance().registerFactory("MockComponent", MockComponent::factory) ; 

    Dependencies::Dependent strict("MockComponent", "MockComponent2") ;
    Component *comp =  Registry::instance().createComponent(strict) ;
    ASSERT_NE(comp, nullptr) ;
    EXPECT_EQ(comp->name(), "MockComponent:MockComponent2") ;

    Dependencies::Dependent defaulted("MockComponent") ;
    comp =  Registry::instance().createComponent(defaulted) ;
    ASSERT_NE(comp, nullptr) ;
    EXPECT_EQ(comp->name(), "MockComponent:MockComponent") ;

    Dependencies::Dependent loose("MockComponent", "") ;
    comp =  Registry::instance().createComponent(loose) ;
    ASSERT_NE(comp, nullptr) ;
    EXPECT_EQ(comp->name(), "MockComponent:MockComponent") ;
}

TEST_F(RegistryTest, TestCreateComponentNotRegisteredThrows)
{
    EXPECT_THROW(Registry::instance().createComponent("Blah"), std::runtime_error) ;
    EXPECT_THROW(Registry::instance().createComponent("Blah","BlahBlah"), std::runtime_error);
    //Dep based returns null;:
    Dependencies::Dependent strict("Blah", "BlahBlah") ;
    ASSERT_EQ(Registry::instance().createComponent(strict), nullptr) ;

}

TEST_F(RegistryTest, ComponentById) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 
    Registry::instance().registerFactory("MockComponent", MockComponent::factory) ; 
    Dependencies::Dependent strict("MockComponent", "MockComponent2") ;
    Component *comp1 =  Registry::instance().createComponent(strict) ;
    Dependencies::Dependent defaulted("MockComponent") ;
    Component *comp2 =  Registry::instance().createComponent(defaulted) ;
    ASSERT_NE(comp1, nullptr) ;
    ASSERT_NE(comp2, nullptr) ;
    ASSERT_NE(comp1, comp2) ;

    //Having established above we can now test access to these compenents by their id

    auto maybe1 = Registry::instance().component(comp1->id()) ;
    ASSERT_TRUE(maybe1) ;
    EXPECT_EQ(maybe1.get(), comp1) ; 

    auto maybe2 = Registry::instance().component(comp2->id()) ;
    ASSERT_TRUE(maybe2) ;
    EXPECT_EQ(maybe2.get(), comp2) ; 

    auto maybeNot = Registry::instance().component(99) ;
    ASSERT_FALSE(maybeNot) ;
}

TEST_F(RegistryTest, ComponentByType) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 
    Registry::instance().registerFactory("MockComponent", MockComponent::factory) ; 
    Dependencies::Dependent strict("MockComponent", "MockComponent2") ;
    Component *comp1 =  Registry::instance().createComponent(strict) ;
    Dependencies::Dependent defaulted("MockComponent") ;
    Component *comp2 =  Registry::instance().createComponent(defaulted) ;
    ASSERT_NE(comp1, nullptr) ;
    ASSERT_NE(comp2, nullptr) ;
    ASSERT_NE(comp1, comp2) ;

    //Having established above we can now test access to these compenents by their type

    auto maybe2 = Registry::instance().component("MockComponent") ;
    ASSERT_TRUE(maybe2) ;
    EXPECT_EQ(maybe2.get(), comp2) ; 

    auto maybeNot = Registry::instance().component("MockComponent3") ;
    ASSERT_FALSE(maybeNot) ;
}

TEST_F(RegistryTest, ComponentByTypeSubType) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 
    Registry::instance().registerFactory("MockComponent", MockComponent::factory) ; 
    Dependencies::Dependent strict("MockComponent", "MockComponent2") ;
    Component *comp1 =  Registry::instance().createComponent(strict) ;
    Dependencies::Dependent defaulted("MockComponent") ;
    Component *comp2 =  Registry::instance().createComponent(defaulted) ;
    ASSERT_NE(comp1, nullptr) ;
    ASSERT_NE(comp2, nullptr) ;
    ASSERT_NE(comp1, comp2) ;

    //Having established above we can now test access to these compenents by their type

    auto maybe1 = Registry::instance().component("MockComponent", "MockComponent2") ;
    ASSERT_TRUE(maybe1) ;
    EXPECT_EQ(maybe1.get(), comp1) ; 

    auto maybe2 = Registry::instance().component("MockComponent", "MockComponent") ;
    ASSERT_TRUE(maybe2) ;
    EXPECT_EQ(maybe2.get(), comp2) ; 

    auto maybeNot = Registry::instance().component("MockComponent2", "MockComponent") ;
    ASSERT_FALSE(maybeNot) ;
}

TEST_F(RegistryTest, ComponentByDep) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 
    Registry::instance().registerFactory("MockComponent", MockComponent::factory) ; 
    Dependencies::Dependent strict("MockComponent", "MockComponent2") ;
    Component *comp1 =  Registry::instance().createComponent(strict) ;
    Dependencies::Dependent defaulted("MockComponent") ;
    Component *comp2 =  Registry::instance().createComponent(defaulted) ;
    Dependencies::Dependent loose("MockComponent", "") ;
    Component *comp3 =  Registry::instance().createComponent(loose) ;

    ASSERT_NE(comp1, nullptr) ;
    ASSERT_NE(comp2, nullptr) ;
    ASSERT_NE(comp3, nullptr) ;
    ASSERT_NE(comp1, comp2) ;
    ASSERT_NE(comp1, comp3) ;

    //Having established above we can now test access to these compenents by their deps

    auto maybe1 = Registry::instance().component(strict) ;
    ASSERT_TRUE(maybe1) ;
    EXPECT_EQ(maybe1.get(), comp1) ; 

    auto maybe2 = Registry::instance().component(defaulted) ;
    ASSERT_TRUE(maybe2) ;
    EXPECT_EQ(maybe2.get(), comp2) ; 

    //Loose is like a box of choclates, yah never know what ya gonna get
    auto maybe3 = Registry::instance().component(loose) ;
    ASSERT_TRUE(maybe3) ;
    EXPECT_EQ(maybe3.get(), comp2) ; 
}

TEST_F(RegistryTest, FactoryMapIteratorWithLocking) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 
    Registry::instance().registerFactory("MockComponent", MockComponent::factory) ; 
    int count = 0 ;
    { //locked
        RegistryLock lock ; 
        for (auto iter = Registry::instance().beginFact(), end = Registry::instance().endFact(); iter!=end; ++iter)
        {
            count++ ;
        }
    } //unlocked
    EXPECT_EQ(count, 2) ;
    // Now make sure operations requiring lock don't block
    Registry::instance().unregisterFactory("MockComponent", "MockComponent2") ; 
    Registry::instance().unregisterFactory("MockComponent") ; 
    count = 0 ;
    { //locked
        RegistryLock lock ; 
        for (auto iter = Registry::instance().beginFact(), end = Registry::instance().endFact(); iter!=end; ++iter)
        {
            count++ ;
        }
    } //unlocked
    EXPECT_EQ(count, 0) ;
}

TEST_F(RegistryTest, LockingWithTimeOut)
{
    bool reached1 = false ;
    bool reached2 = false ;
    { //locked
        RegistryLock lock ; 
        reached1 = true ;
        try {
            RegistryLock lock2(1,1) ;
            reached2 = true ;

        } 
        catch(RegistryLocked ex) 
        {
            //empty
        }
    } //unlocked
    EXPECT_TRUE(reached1) ;;
    EXPECT_FALSE(reached2) ;;
}

namespace {
bool contains(const std::string& a, const std::string& b) {return a.find(b) !=  std::string::npos;}
}

TEST_F(RegistryTest, Dump) 
{
    Registry::instance().registerFactory("MockComponent", "MockComponent2", MockComponent2::factory) ; 
    Registry::instance().registerFactory("MockComponent", MockComponent::factory) ; 
    Dependencies::Dependent strict("MockComponent", "MockComponent2") ;
    Registry::instance().createComponent(strict) ;
    Dependencies::Dependent defaulted("MockComponent") ;
    Registry::instance().createComponent(defaulted) ;
    Dependencies::Dependent loose("MockComponent", "") ;
    Registry::instance().createComponent(loose) ;
    std::ostringstream oss ;
    Registry::instance().dump(oss) ;
    EXPECT_PRED2(contains, oss.str(), "Factories {") ;
    EXPECT_PRED2(contains, oss.str(), "(MockComponent, MockComponent)") ;
    EXPECT_PRED2(contains, oss.str(), "(MockComponent, MockComponent2)") ;
    EXPECT_PRED2(contains, oss.str(), "CompXRefs {") ;
}

