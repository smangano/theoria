#include <theoria/core/CoreComponents.h>
#include <theoria/config/Config.h>
#include <theoria/config/TOMLConfigBuilder.h>
#include <theoria/core/Registry.h>
#include <theoria/core/Component.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace theoria { 

const char * ConfigVarResolverBuilderTOML = R"toml(
[ConfigVarResolverBuilder]                    
desc="""
      Creates the chain of ConfigVarResolver's used by the ConfigBuilder. The cofiguration specifies the ordered
      list of resolvers to chain together.
"""
kind="Component"
[[ConfigVarResolverBuilder.Resolver]]         
type="ConfigVarResolver"
subtype="CmdLineResolver"
[[ConfigVarResolverBuilder.Resolver]]         
type="ConfigVarResolver"
subtype="EnvVarResolver"
[[ConfigVarResolverBuilder.Resolver]]         
type="ConfigVarResolver"
subtype="ConfigResolver"
)toml" ;

class TestConfigVarResolverBuilderComp : public ::testing::Test
{
    virtual void SetUp() {
        core::Registry::instance().registerFactory("ConfigVarResolverBuilder", core::ConfigVarResolverBuilderComp::factory) ;
        std::istringstream iss(ConfigVarResolverBuilderTOML) ;
        cfg = config::TOMLConfigBuilder().parse(iss) ; 
        builder = core::Registry::instance().createComponent("ConfigVarResolverBuilder") ;
    }

    virtual void TearDown() {
        core::Registry::instance().reset() ;
    }

public:

    std::unique_ptr<const config::Config> cfg ;
    core::Component* builder ;
} ;

TEST_F(TestConfigVarResolverBuilderComp, TestInit) {
    const config::Config* configVarResolverBuilder = cfg->getChild("ConfigVarResolverBuilder") ;
    ASSERT_NE(configVarResolverBuilder, nullptr) ;
    core::Dependencies deps = builder->init(*configVarResolverBuilder) ;
}



} //namespace theoria
