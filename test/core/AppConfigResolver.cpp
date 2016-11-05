#include "theoria/core/AppConfigResolver.h"
#include "theoria/config/Config.h"
#include "theoria/config/Builder.h"
#include <gtest/gtest.h>

#define private public
#include "theoria/core/Registry.h"
#undef private

using namespace theoria ;
using namespace theoria::config ;
using namespace theoria::core ;

namespace {

class BuildTestConfig : public ConfigBuilder
{
public:
    Config* testConfigWithMultipleChildren() ;
} ;

Config* BuildTestConfig::testConfigWithMultipleChildren() 
{
    pushConfig("Test", "testConfigWithMultipleChildren") ;
    addAttr("attr1", "1", "int") ;
    addAttr("attr2", "foo", "string") ;
    pushConfig("Child1", "Some Child") ;
    addAttr("attr1", "2", "int") ;
    popAsChild() ;
    pushConfig("Child2", "Some Other Child") ;
    addAttr("attr1", "3", "int") ;
    popAsChild() ;
    pushConfig("Child3", "Yet Another Child") ;
    popAsChild() ;
    return releaseAll() ;
}

} //namespace

class AppConfigResolverTest : public ::testing::Test 
{
    virtual void SetUp() {
       _builder = new BuildTestConfig() ;  
    }

    virtual void TearDown() {
        delete _builder ;
        Registry::instance().reset() ;
    }

public:

    BuildTestConfig& builder() {return *_builder;}
    
protected:

    BuildTestConfig *_builder ;
    
} ;

TEST_F(AppConfigResolverTest, TestLookup) {
    std::unique_ptr<const Config> p(builder().testConfigWithMultipleChildren()) ;
    Registry::instance()._setAppConfig(p) ;
    AppConfigResolver appConfigResolver ;

    EXPECT_EQ(appConfigResolver.name(), "AppConfigResolver") ;

    auto result = appConfigResolver.lookup("attr1") ;
    EXPECT_EQ(result.first, &appConfigResolver) ;
    EXPECT_EQ(result.second, "1") ;

    result = appConfigResolver.lookup("Child1.attr1") ;
    EXPECT_EQ(result.first, &appConfigResolver) ;
    EXPECT_EQ(result.second, "2") ;

    result = appConfigResolver.lookup("notthere") ;
    EXPECT_EQ(result.first, nullptr) ;
    EXPECT_EQ(result.second, "") ;
}
