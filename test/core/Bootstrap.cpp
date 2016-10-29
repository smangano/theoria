#include <theoria/core/Bootstrap.h>
#include <theoria/core/Component.h>
#include <theoria/core/Registry.h>
#include <theoria/core/RegisterThis.h>
#include <theoria/config/Config.h>
#include <theoria/config/TOMLConfigBuilder.h>
#include <gtest/gtest.h>

namespace theoria {

#include "fixtures/core/mockComponents.cpp"

class TestBootstrap : public ::testing::Test
{
    virtual void SetUp() {
        std::istringstream iss(TOML_CONFIG) ;
        config = config::TOMLConfigBuilder().parse(iss) ; 
        MockComponent::reg();
        MockComponent2::reg();
    }
    virtual void TearDown() {
        core::Registry::instance().reset() ;
    } 

public:

    std::unique_ptr<const config::Config> config ;
    core::Bootstrap bootstrap ;
} ;

TEST_F(TestBootstrap, createCoreCompTest) {
    bootstrap._createCoreComp(*config->getChild("MockComponent")) ; 
    auto maybe = core::Registry::instance().component("MockComponent") ;
    ASSERT_TRUE(maybe) ;
}

}
