#include <theoria/core/Bootstrap.h>
#include <theoria/core/Component.h>
#include <theoria/core/Registry.h>
#include <theoria/core/RegisterThis.h>
#include <theoria/config/Config.h>
#include <theoria/config/TOMLConfigBuilder.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace theoria {

#include "fixtures/core/mockComponents.cpp"

class GoogleMockComponent : public core::Component
{
public:
    
    MOCK_METHOD1(appLifeCycle, void(core::AppLifeCycle state));
} ;

class MyMockComponent : public core::Component
{
public:
    MyMockComponent():
        count(0), lastCall(core::AppLifeCycle::NONE) {}

    void appLifeCycle(core::AppLifeCycle state) ;

    int count  ;
    core::AppLifeCycle lastCall ;
};

void MyMockComponent::appLifeCycle(core::AppLifeCycle state)
{
    count++ ;
    lastCall = state ;
}

class TestBootstrap : public ::testing::Test
{
    virtual void SetUp() {
        std::istringstream iss(TOML_CONFIG) ;
        config = config::TOMLConfigBuilder().parse(iss) ; 
        MockComponent::reg();
        MockComponent2::reg();
        componentConfigs = 
            config->getChildren( [] (const config::Config* child) {return child->getAttrAsStr("kind", "Component") == "Component";} ) ;
    }
    virtual void TearDown() {
        core::Registry::instance().reset() ;
    } 

public:

    std::unique_ptr<const config::Config> config ;
    core::Bootstrap bootstrap ;
    core::Bootstrap::ConstConfigList componentConfigs ;
} ;

TEST_F(TestBootstrap, createCoreCompTest) {
    bootstrap._createCoreComp(*config->getChild("MockComponent")) ; 
    auto maybe = core::Registry::instance().component("MockComponent") ;
    ASSERT_TRUE(maybe) ;
    auto maybe2 = core::Registry::instance().component("MockComponent", "MockComponent2") ;
    ASSERT_FALSE(maybe2) ; //not created yet
    bootstrap._createCoreComp(*config->getChild("MockComponent2")) ; 
    maybe2 = core::Registry::instance().component("MockComponent", "MockComponent2") ;
    ASSERT_TRUE(maybe2) ;
}

TEST_F(TestBootstrap, createCoreCompentsTest) {
    std::vector<core::Component*> comps ;
    bootstrap.createCoreComponents(componentConfigs, comps) ;
    ASSERT_EQ(comps.size(), 2) ;
    auto maybeMC = core::Registry::instance().component("MockComponent") ; 
    ASSERT_TRUE(maybeMC) ;
    core::Component& mockComp = *maybeMC ;
    theoria::MockComponent* mc = mockComp.cast<theoria::MockComponent>() ;
    ASSERT_NE(mc, nullptr) ;
}

TEST_F(TestBootstrap, initializeCoreComponentTest) {
    std::vector<core::Component*> comps ;
    bootstrap.createCoreComponents(componentConfigs, comps) ;
    std::unordered_map<core::CompId, core::Dependencies> coreCompDeps ;
    bootstrap.initializeCoreComponent(componentConfigs, comps, coreCompDeps) ;
    ASSERT_EQ(coreCompDeps.size(), 2) ;
    ASSERT_NE(coreCompDeps.find(comps[0]->id()), coreCompDeps.end()) ;
    ASSERT_NE(coreCompDeps.find(comps[1]->id()), coreCompDeps.end()) ;
}

TEST_F(TestBootstrap, initializeAppLifeCycleTest) {

    //Google mock not working here! Not sure why

    //GoogleMockComponent* gmComp = new GoogleMockComponent();
    MyMockComponent* myMock = new MyMockComponent() ;
    std::vector<core::Component*> components;
    components.push_back(myMock) ;
    bootstrap.initializeAppLifeCycle(components) ;
    //EXPECT_CALL(*gmComp,appLifeCycle(core::AppLifeCycle::INITIALIZED)) ;
    EXPECT_EQ(myMock->count, 1) ;
    EXPECT_EQ(myMock->lastCall, core::AppLifeCycle::INITIALIZED) ;
    //delete gmComp ;
    delete myMock ;
}

TEST_F(TestBootstrap, finalizeAppLifeCycleTest) {
    MyMockComponent* myMock = new MyMockComponent() ;
    std::vector<core::Component*> components;
    components.push_back(myMock) ;
    bootstrap.finalizeAppLifeCycle(components) ;
    EXPECT_EQ(myMock->count, 1) ;
    EXPECT_EQ(myMock->lastCall, core::AppLifeCycle::FINALIZED) ;
    delete myMock ;
}

} //namespace theoria
