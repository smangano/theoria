#include <theoria/core/Bootstrap.h>
#include <theoria/core/Component.h>
#include <theoria/core/Registry.h>
#include <theoria/core/RegisterThis.h>
#include <theoria/config/Config.h>
#include <theoria/config/TOMLConfigBuilder.h>
#include <theoria/os/os.h>
#include <theoria/util/CommandLine.h>
#include <theoria/envvars.h>
#include <theoria/options.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdlib>

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

TEST_F(TestBootstrap, satisfyAndFinalizeTest) {
    std::vector<core::Component*> comps ;
    bootstrap.createCoreComponents(componentConfigs, comps) ;
    std::unordered_map<core::CompId, core::Dependencies> coreCompDeps ;
    bootstrap.initializeCoreComponent(componentConfigs, comps, coreCompDeps) ;
    bootstrap.satisfyAndFinalize(coreCompDeps) ;
    auto maybeMC = core::Registry::instance().component("MockComponent", "MockComponent") ; 
    ASSERT_TRUE(maybeMC) ;
    EXPECT_EQ(maybeMC->cast<MockComponent>()->myDeps.size(), 0) ;
    auto maybeMC2 = core::Registry::instance().component("MockComponent", "MockComponent2") ; 
    ASSERT_TRUE(maybeMC2) ;
    ASSERT_EQ(maybeMC2->cast<MockComponent2>()->myDeps.size(), 1) ;
    EXPECT_EQ(maybeMC2->cast<MockComponent2>()->myDeps[0], maybeMC.get()) ;
}

TEST(TestBootstrap1, TestSingleCompontConfig) {
    std::istringstream iss(TOML_CONFIG1) ;
    auto config = config::TOMLConfigBuilder().parse(iss) ; 
    auto componentConfigs = 
        config->getChildren( [] (const config::Config* child) {return child->getAttrAsStr("kind", "Component") == "Component";} ) ;
    MockComponent::reg();
    std::vector<core::Component*> comps ;
    core::Bootstrap bootstrap ;
    bootstrap.createCoreComponents(componentConfigs, comps) ;
    std::unordered_map<core::CompId, core::Dependencies> coreCompDeps ;
    bootstrap.initializeCoreComponent(componentConfigs, comps, coreCompDeps) ;
    bootstrap.satisfyAndFinalize(coreCompDeps) ;
    auto maybeMC = core::Registry::instance().component("MockComponent") ; 
    ASSERT_TRUE(maybeMC) ;
    EXPECT_EQ(maybeMC->cast<MockComponent>()->myDeps.size(), 0) ;
}

class TempFile
{
public:
    
    TempFile(const std::string& path):
        _path(path) 
    {
        FILE* fp ;
        if (!(fp=fopen(_path.c_str(), "w")))
            std::runtime_error("TempFile create failed") ; 
        fclose(fp) ;
    }

    ~TempFile() {
        remove(_path.c_str()) ;
    }

    std::string path() const {return _path;} 

private:

    std::string _path ;
} ;

class BootstrapFindConfigTest : public ::testing::Test
{

    virtual void TearDown() {
        util::CommandLine::reset() ; 
    }
} ;

TEST_F(BootstrapFindConfigTest, findConfigViaEnv) {
    const char* argv[] = {"dummy.toml"} ;
    bool allowMissingConfig = true ;
    util::CommandLine cl(1, argv, allowMissingConfig) ;
    TempFile temp("./bootstrap.toml") ;
    setenv(ENV_BOOTSTRAP_CONFIG_PATH, "/tmp:.", 1) ;
    core::Bootstrap bootstrap;
    EXPECT_EQ(bootstrap.findConfig(), temp.path()) ;
    unsetenv(ENV_BOOTSTRAP_CONFIG_PATH) ;
}

TEST_F(BootstrapFindConfigTest, findConfigViaCmdLineOpt) {
    std::string buf ;
    const char* argv[] = {"dummy.toml", OPT(buf, OPTION_BOOTSTRAP_CONFIG_PATH[0]), "."} ;
    bool allowMissingConfig = true ;
    util::CommandLine cl(3, argv, allowMissingConfig) ;
    TempFile temp("./bootstrap.toml") ;
    unsetenv(ENV_BOOTSTRAP_CONFIG_PATH) ;
    core::Bootstrap bootstrap;
    EXPECT_EQ(bootstrap.findConfig(), temp.path()) ;
}

TEST_F(BootstrapFindConfigTest, findConfigViaOS) {
    
    if (os::dir_exists("/usr/local/theoria") && !os::exists("/usr/local/theoria/bootstrap.toml")) {
        std::string buf ;
        const char* argv[] = {"dummy.toml"} ;
        bool allowMissingConfig = true ;
        util::CommandLine cl(1, argv, allowMissingConfig) ;
        TempFile temp("/usr/local/theoria/bootstrap.toml") ;
        unsetenv(ENV_BOOTSTRAP_CONFIG_PATH) ;
        core::Bootstrap bootstrap;
        EXPECT_EQ(bootstrap.findConfig(), temp.path()) ;
    }
}

TEST_F(BootstrapFindConfigTest, findConfigThrows) {
    
    if (!os::exists("/usr/local/theoria/bootstrap.toml")) {
        std::string buf ;
        const char* argv[] = {"dummy.toml"} ;
        bool allowMissingConfig = true ;
        util::CommandLine cl(1, argv, allowMissingConfig) ;
        unsetenv(ENV_BOOTSTRAP_CONFIG_PATH) ;
        core::Bootstrap bootstrap;
        EXPECT_THROW(bootstrap.findConfig(), std::runtime_error) ;
    }
}


} //namespace theoria
