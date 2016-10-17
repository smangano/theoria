#include "theoria/config/Config.h"
#include "theoria/config/TOMLConfigBuilder.h"
#include <gtest/gtest.h>

using namespace theoria ;
using namespace theoria::config ;

const std::string TEST1 = "                \n\
application=\"Test1App\"                   \n\
desc=\"A Test of TOML\"                    \n\
                                           \n\
[someConfig]                               \n\
a=1                                        \n\
b=1.5                                      \n\
c=1979-05-27T07:32:00-08:00                \n\
d=true                                     \n\
" ;

const std::string TEST2 = "                \n\
application=\"Test1App\"                   \n\
desc=\"A Test of TOML\"                    \n\
                                           \n\
[[someConfig]]                             \n\
a=1                                        \n\
b=1.5                                      \n\
c=1979-05-27T07:32:00-08:00                \n\
d=true                                     \n\
                                           \n\
[[someConfig]]                             \n\
a=17                                       \n\
b=100.5                                    \n\
c=2016-10-15T10:00:00                      \n\
d=false                                    \n\
" ;

class TOMLConfigBuilderTest : public ::testing::Test 
{
    virtual void SetUp() {
       _builder = new TOMLConfigBuilder() ;  
    }

    virtual void TearDown() {
        delete _builder ;
    }

protected:

    TOMLConfigBuilder& builder() {return *_builder;}
    
    TOMLConfigBuilder *_builder ;
    
} ;

TEST_F(TOMLConfigBuilderTest, TestSimpleTOML) {
    std::istringstream iss(TEST1) ;
   const Config* config =  builder().parse(iss) ;
   ASSERT_EQ(config->name(), "Test1App") ;
   ASSERT_EQ(config->desc(), "A Test of TOML") ;
   ASSERT_EQ(config->getParent(), nullptr) ;
   ASSERT_EQ(config->numChildren(), 1) ;
   ASSERT_FALSE(config->isArray()) ;
   Config::ConstConfigList children = config->getChildren() ;
   const Config* child = children[0] ;
   ASSERT_EQ(child->name(), "someConfig") ;
   ASSERT_TRUE(child->isArray()) ;
   ASSERT_EQ(child->getAttrAsInt("a"), 1) ;
   ASSERT_EQ(child->getAttrAsDbl("b"), 1.5) ;
   ASSERT_EQ(child->getAttrAsStr("c"), "1979-05-27T07:32:00-08:00") ;
   ASSERT_EQ(child->getAttr<bool>("d"), true) ;
}

TEST_F(TOMLConfigBuilderTest, TestTOMLWithTableArray) {
   std::istringstream iss(TEST2) ;
   std::cout << iss.str() << std::endl ;
   const Config* config =  builder().parse(iss) ;
   ASSERT_EQ(config->name(), "Test1App") ;
   ASSERT_EQ(config->desc(), "A Test of TOML") ;
   ASSERT_EQ(config->getParent(), nullptr) ;
   ASSERT_EQ(config->numChildren(), 1) ;
   Config::ConstConfigList children = config->getChildren() ;
   const Config* child = children[0] ;
   ASSERT_EQ(child->name(), "someConfig") ;
   ASSERT_TRUE(child->isArray()) ;
   const ConfigArray* array = static_cast<const ConfigArray*>(child) ;
   ASSERT_EQ(array->numElements(), 2) ;
   const Config* elem1 = array->at(0) ;
   const Config* elem2 = array->at(1) ;
   ASSERT_EQ(elem1->name(), "someConfig") ;
   ASSERT_EQ(elem2->name(), "someConfig") ;
}
