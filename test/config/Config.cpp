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

Config* BuildTestConfig::testSingleConfigWithAttr()
{
    pushConfig("Test", "testSingleConfigWithAttr") ;
    addAttr("attr1", "1", "int") ;
    addAttr("attr2", "foo", "string") ;
    return releaseAll() ;
}

Config* BuildTestConfig::testConfigWithOneChild() 
{
    pushConfig("Test", "testConfigWithOneChild") ;
    addAttr("attr1", "1", "int") ;
    addAttr("attr2", "foo", "string") ;
    pushConfig("Child1", "Some Child") ;
    addAttr("attr1", "2", "int") ;
    popAsChild() ;
    return releaseAll() ;
}

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

class ConfigTest : public ::testing::Test 
{
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
    Config * config = builder().testSingleConfigNoAttr() ;
    ASSERT_EQ(config->name(), "Test") ;
    ASSERT_EQ(config->desc(), "testSingleConfigNoAttr") ;
    ASSERT_EQ(config->getParent(), nullptr) ;
    ASSERT_EQ(config->getChildren(), Config::ConstConfigList()) ;
    ASSERT_EQ(config->numAttr(), 0) ;
    ASSERT_TRUE(config->isRoot()) ;
    ASSERT_TRUE(config->isLeaf()) ;
}

TEST_F(ConfigTest, SingleConfigWithAttrTest) {
    Config * config = builder().testSingleConfigWithAttr() ;
    ASSERT_EQ(config->name(), "Test") ;
    ASSERT_EQ(config->desc(), "testSingleConfigWithAttr") ;
    ASSERT_EQ(config->getParent(), nullptr) ;
    ASSERT_EQ(config->getChildren(), Config::ConstConfigList()) ;
    ASSERT_EQ(config->numAttr(), 2) ;
    ASSERT_TRUE(config->hasAttr("attr1")) ;
    ASSERT_EQ(config->getAttrAsStr("attr1"), "1") ;
    ASSERT_EQ(config->getAttrAsInt("attr1"), 1) ;
    ASSERT_TRUE(config->hasAttr("attr2")) ;
    ASSERT_EQ(config->getAttrAsStr("attr2"), "foo") ;
    ASSERT_TRUE(config->isRoot()) ;
    ASSERT_TRUE(config->isLeaf()) ;
}

TEST_F(ConfigTest, SingleConfigWithOneChildTest) {
    Config * config = builder().testConfigWithOneChild() ;
    ASSERT_EQ(config->name(), "Test") ;
    ASSERT_EQ(config->desc(), "testConfigWithOneChild") ;
    ASSERT_EQ(config->getParent(), nullptr) ;
    ASSERT_EQ(config->numChildren(), 1) ;
    ASSERT_EQ(config->numAttr(), 2) ;
    ASSERT_TRUE(config->hasAttr("attr1")) ;
    ASSERT_EQ(config->getAttrAsStr("attr1"), "1") ;
    ASSERT_EQ(config->getAttrAsInt("attr1"), 1) ;
    ASSERT_TRUE(config->hasAttr("attr2")) ;
    ASSERT_EQ(config->getAttrAsStr("attr2"), "foo") ;
    ASSERT_TRUE(config->isRoot()) ;
    ASSERT_FALSE(config->isLeaf()) ;

    const Config * child = config->getChildren()[0] ;
    ASSERT_EQ(child->name(), "Child1") ;
    ASSERT_EQ(child->desc(), "Some Child") ;
    ASSERT_EQ(child->getParent(), config) ;
    ASSERT_EQ(child->numChildren(), 0) ;
    ASSERT_EQ(child->numAttr(), 1) ;
    ASSERT_TRUE(child->hasAttr("attr1")) ;
    ASSERT_EQ(child->getAttrAsInt("attr1"), 2) ;
    ASSERT_TRUE(config->hasChild(child)) ;
    ASSERT_EQ(child->getSiblings(), Config::ConstConfigList()) ;
    ASSERT_FALSE(child->isRoot()) ;
    ASSERT_TRUE(child->isLeaf()) ;
}

TEST_F(ConfigTest, SingleConfigWithMultipleChildrenTest) {
    Config * config = builder().testConfigWithMultipleChildren() ;
    ASSERT_EQ(config->name(), "Test") ;
    ASSERT_EQ(config->desc(), "testConfigWithMultipleChildren") ;
    ASSERT_EQ(config->getParent(), nullptr) ;
    ASSERT_EQ(config->numChildren(), 3) ;
    ASSERT_EQ(config->numAttr(), 2) ;
    ASSERT_TRUE(config->hasAttr("attr1")) ;
    ASSERT_EQ(config->getAttrAsStr("attr1"), "1") ;
    ASSERT_EQ(config->getAttrAsInt("attr1"), 1) ;
    ASSERT_TRUE(config->hasAttr("attr2")) ;
    ASSERT_EQ(config->getAttrAsStr("attr2"), "foo") ;

    auto children = config->getChildren() ;
    const Config * child1 = children[0] ;
    const Config * child2 = children[1] ;
    const Config * child3 = children[2] ;

    ASSERT_EQ(child1->name(), "Child1") ;
    ASSERT_EQ(child2->name(), "Child2") ;
    ASSERT_EQ(child3->name(), "Child3") ;

    ASSERT_EQ(child1->desc(), "Some Child") ;
    ASSERT_EQ(child2->desc(), "Some Other Child") ;
    ASSERT_EQ(child3->desc(), "Yet Another Child") ;

    ASSERT_EQ(child1->getParent(), config) ;
    ASSERT_EQ(child2->getParent(), config) ;
    ASSERT_EQ(child3->getParent(), config) ;

    ASSERT_EQ(child1->numChildren(), 0) ;
    ASSERT_EQ(child2->numChildren(), 0) ;
    ASSERT_EQ(child3->numChildren(), 0) ;

    ASSERT_EQ(child1->numAttr(), 1) ;
    ASSERT_EQ(child2->numAttr(), 1) ;
    ASSERT_EQ(child3->numAttr(), 0) ;

    ASSERT_TRUE(config->hasChild(child1)) ;
    ASSERT_TRUE(config->hasChild(child2)) ;
    ASSERT_TRUE(config->hasChild(child3)) ;

    Config::ConstConfigList child1Sibs = {child2, child3} ;
    ASSERT_EQ(child1->getSiblings(), child1Sibs) ;

    Config::ConstConfigList child2Sibs = {child1, child3} ;
    ASSERT_EQ(child2->getSiblings(), child2Sibs) ;

    Config::ConstConfigList child3Sibs = {child1, child2} ;
    ASSERT_EQ(child3->getSiblings(), child3Sibs) ;

    Config::ConstConfigList child3SibsFiltered = {child2} ;
    Config::ConstConfigList filtered = child3->getSiblings([&] (const Config* sib) {return sib->name() != "Child1";}) ;
    ASSERT_EQ(filtered, child3SibsFiltered) ;


}
