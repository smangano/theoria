#include "theoria/config/Config.h"
#include "theoria/config/Builder.h"
#include <gtest/gtest.h>

using namespace theoria ;
using namespace theoria::config ;

class BuildTestConfig : private ConfigBuilder
{
public:

    Config* testSingleConfigNoAttr() ;
    Config* testSingleConfigWithAttr() ;
    Config* testConfigWithOneChild() ;
    Config* testConfigWithMultipleChildren() ;
} ;

Config* BuildTestConfig::testSingleConfigNoAttr()
{
    pushConfig("Test", "testSingleConfigNoAttr") ;
    return releaseAll() ;
}

class ConfigTest : public ::testing::Test 

    virtual void SetUp() {
       _builder = new BuildTestConfig() ;  
    }

    virtual void TearDown() {
        delete _builder ;
    }

protected:

    BuildTestConfig& builder() {return *_builder;}
    
    BuildTestConfig *_builder ;
    
} ;

TEST_F(ConfigTest, SingleConfigTest) {
    Config * config = testSingleConfigNoAttr() ;
    ASSERT_EQ(config->name(), "Test") ;
    ASSERT_EQ(config->desc(), "testSingleConfigNoAttr") ;
}
