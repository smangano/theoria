#include "theoria/config/Config.h"
#include "theoria/config/TOMLConfigBuilder.h"
#include <gtest/gtest.h>

using namespace theoria ;
using namespace theoria::config ;

const std::string TEST1 = "                 \n\
application=\"Test1App\"                    \n\
desc=\"A Test of TOML\"                     \n\
                                            \n\
[someConfig]                                \n\
a=1                                         \n\
b=1.5                                       \n\
c=1979-05-27T07:32:00-08:00                 \n\
d=true                                      \n\
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
    std::unique_ptr<const Config> config ;
    
} ;

TEST_F(TOMLConfigBuilderTest, TestSimpleTOML) {
    std::istringstream iss(TEST1) ;
   config = std::move(builder().parse(iss)) ;
   ASSERT_EQ(config->name(), "Test1App") ;
   ASSERT_EQ(config->desc(), "A Test of TOML") ;
   ASSERT_EQ(config->getParent(), nullptr) ;
   ASSERT_EQ(config->numChildren(), 1) ;
   ASSERT_FALSE(config->isArray()) ;
   Config::ConstConfigList children = config->getChildren() ;
   const Config* child = children[0] ;
   ASSERT_EQ(child->name(), "someConfig") ;
   ASSERT_FALSE(child->isArray()) ;
   ASSERT_EQ(child->getAttrAsInt("a"), 1) ;
   ASSERT_EQ(child->getAttrAsDbl("b"), 1.5) ;
   ASSERT_EQ(child->getAttrAsStr("c"), "1979-05-27T07:32:00-08:00") ;
   ASSERT_EQ(child->getAttr<bool>("d"), true) ;
}

const std::string TEST2 = "                 \n\
application=\"Test1App\"                    \n\
desc=\"A Test of TOML\"                     \n\
                                            \n\
[[someConfig]]                              \n\
a=1                                         \n\
b=1.5                                       \n\
c=1979-05-27T07:32:00-08:00                 \n\
d=true                                      \n\
                                            \n\
[[someConfig]]                              \n\
a=17                                        \n\
b=100.5                                     \n\
c=2016-10-15T10:00:00Z                      \n\
d=false                                     \n\
" ;


TEST_F(TOMLConfigBuilderTest, TestTOMLWithTableArray) {
   std::istringstream iss(TEST2) ;
   config =  std::move(builder().parse(iss)) ;
   ASSERT_EQ(config->name(), "Test1App") ;
   ASSERT_EQ(config->desc(), "A Test of TOML") ;
   ASSERT_EQ(config->getParent(), nullptr) ;
   ASSERT_EQ(config->numChildren(), 1) ;
   Config::ConstConfigList children = config->getChildren() ;
   const Config* child = children[0] ;
   ASSERT_EQ(child->name(), "someConfig_Array") ;
   ASSERT_TRUE(child->isArray()) ;
   const ConfigArray* array = static_cast<const ConfigArray*>(child) ;
   ASSERT_EQ(array->numElements(), 2) ;
   const Config* elem1 = array->at(0) ;
   const Config* elem2 = array->at(1) ;
   ASSERT_EQ(elem1->name(), "someConfig") ;
   ASSERT_EQ(elem2->name(), "someConfig") ;
}

const std::string TEST3 = "                 \n\
[A]                                         \n\
a=1                                         \n\
[A.B]                                       \n\
b=2                                         \n\
[A.B.C]                                     \n\
c=3                                         \n\
[[A.B.X]]                                   \n\
x=1                                         \n\
[[A.B.X]]                                   \n\
x=2                                         \n\
" ;

TEST_F(TOMLConfigBuilderTest, TestTOMLWithNestedTables) {
    std::istringstream iss(TEST3) ;
    config =  std::move(builder().parse(iss)) ;
    EXPECT_EQ(config->getParent(), nullptr) ;
    ASSERT_EQ(config->numChildren(), 1) ;
    EXPECT_FALSE(config->isArray()) ;
    Config::ConstConfigList children = config->getChildren() ;
    const Config* child = children[0] ;
    ASSERT_EQ(child->numChildren(), 1) ;
    EXPECT_FALSE(child->isArray()) ;
    EXPECT_EQ(child->name(),"A") ;
    Config::ConstConfigList children2 = child->getChildren() ;
    const Config* child2 = children2[0] ;
    EXPECT_EQ(child2->name(),"B") ;
    ASSERT_EQ(child2->numChildren(), 2) ; //Table A.B.C and ARRAY A.B.X
    Config::ConstConfigList children3 = child2->getChildren() ;
    const Config* child3_1 = children3[0] ;
    const Config* child3_2 = children3[1] ;
    EXPECT_TRUE(child3_1->isArray() || child3_2->isArray()) ;
    //TOML is unorderd so can't expect order here
    if (child3_1->isArray()) {
        std::swap(child3_1, child3_2) ;
    }
    EXPECT_FALSE(child3_1->isArray()) ;
    EXPECT_TRUE(child3_2->isArray()) ;
    EXPECT_EQ(child3_1->name(),"C") ;
    EXPECT_EQ(child3_2->name(),"X_Array") ;
    const ConfigArray* array = static_cast<const ConfigArray*>(child3_2) ;
    ASSERT_EQ(array->numElements(), 2) ;
    const Config* elem1 = array->at(0) ;
    const Config* elem2 = array->at(1) ;
    EXPECT_EQ(elem1->name(), "X") ;
    EXPECT_EQ(elem2->name(), "X") ;
}    


 
