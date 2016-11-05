#include <theoria/config/Config.h>
#include <theoria/config/TOMLConfigBuilder.h>
#include <theoria/core/Registry.h>
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
        std::istringstream iss(ConfigVarResolverBuilderTOML) ;
        config = config::TOMLConfigBuilder().parse(iss) ; 
    }

    virtual void TearDown() {
        core::Registry::instance().reset() ;
    }

    std::unique_ptr<const config::Config> config ;
} ;

} //namespace theoria
