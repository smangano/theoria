#include <theoria/core/CoreComponents.h>
#include <theoria/config/Config.h>
#include <theoria/config/TOMLConfigBuilder.h>
#include <theoria/config/Resolve.h>
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
type="ConfigVariableResolver"
subtype="CmdLineResolver"
[[ConfigVarResolverBuilder.Resolver]]         
type="ConfigVariableResolver"
subtype="EnvVarResolver"
[[ConfigVarResolverBuilder.Resolver]]         
type="ConfigVariableResolver"
subtype="AppConfigResolver"

[ConfigBuilder]
kind="Component"
type="ConfigBuilder"
subtype="TOMLConfigBuilder"

)toml" ;

class TestConfigVarResolverBuilderComp : public ::testing::Test
{
    virtual void SetUp() {
        core::Registry::instance().registerFactory("ConfigBuilder", core::TOMLConfigBuilderComp::factory) ;
        core::Registry::instance().registerFactory("ConfigVarResolverBuilder", core::ConfigVarResolverBuilderComp::factory) ;
        core::Registry::instance().registerFactory("ConfigVariableResolver", "EnvVarResolver", core::EnvVarResolverComp::factory) ;
        core::Registry::instance().registerFactory("ConfigVariableResolver", "CmdLineResolver", core::CmdLineResolverComp::factory) ;
        core::Registry::instance().registerFactory("ConfigVariableResolver", "AppConfigResolver", core::AppConfigResolverComp::factory) ;
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
    ASSERT_EQ(deps.size(), 4) ; //First dep is loose to ConfigBuilder
    EXPECT_EQ(deps[0].type, "ConfigBuilder") ;
    EXPECT_EQ(deps[1].type, "ConfigVariableResolver") ;
    EXPECT_EQ(deps[1].subtype, "CmdLineResolver") ;
    EXPECT_EQ(deps[2].type, "ConfigVariableResolver") ;
    EXPECT_EQ(deps[2].subtype, "EnvVarResolver") ;
    EXPECT_EQ(deps[3].type, "ConfigVariableResolver") ;
    EXPECT_EQ(deps[3].subtype, "AppConfigResolver") ;
}

TEST_F(TestConfigVarResolverBuilderComp, TestFinalize) {
    const config::Config* configVarResolverBuilder = cfg->getChild("ConfigVarResolverBuilder") ;
    core::Dependencies deps = builder->init(*configVarResolverBuilder) ;
    auto& reg = core::Registry::instance() ;
    std::vector<core::Component*> comps ;
    core::Component* configBuilder = reg.createComponent(deps[0]) ;  
    ASSERT_NE(configBuilder, nullptr) ;
    comps.push_back(configBuilder) ;
    core::Component* resolver1 = reg.createComponent(deps[1]) ;  
    ASSERT_NE(resolver1, nullptr) ;
    comps.push_back(resolver1) ;
    core::Component* resolver2 = reg.createComponent(deps[2]) ;  
    ASSERT_NE(resolver2, nullptr) ;
    comps.push_back(resolver2) ;
    core::Component* resolver3 = reg.createComponent(deps[3]) ;  
    ASSERT_NE(resolver3, nullptr) ;
    comps.push_back(resolver3) ;
    
    builder->finalize(comps) ;
    config::ConfigBuilder* configBuilderImpl = configBuilder->cast<config::ConfigBuilder>() ;
    EXPECT_EQ(configBuilderImpl->resolver(), resolver1->cast<config::ConfigVariableResolver>()) ; 
    EXPECT_EQ(configBuilderImpl->resolver()->next(), resolver2->cast<config::ConfigVariableResolver>()) ; 
    EXPECT_EQ(configBuilderImpl->resolver()->next()->next(), resolver3->cast<config::ConfigVariableResolver>()) ; 
    EXPECT_EQ(configBuilderImpl->resolver()->next()->next()->next(), nullptr) ; 
}

} //namespace theoria
