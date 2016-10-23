#include <theoria/core/Dependencies.h>
#include <gtest/gtest.h>


using namespace theoria ;
using namespace core ;

class DependentTest : public ::testing::Test 
{
public:

    Dependencies::Dependent strict {"Type1", "SubType1"} ;   
    Dependencies::Dependent defaulted {"Type1"} ;   
    Dependencies::Dependent loose {"Type1", ""} ;   
    bool optional = true;
    Dependencies::Dependent strictOpt {"Type1", "SubType1", optional} ;   
    Dependencies::Dependent defaultedOpt {"Type1", optional} ;   
    Dependencies::Dependent looseOpt {"Type1", "", optional} ;   
} ;

TEST_F(DependentTest, TestStr) {
    EXPECT_EQ(strict.str(), "Dependent(Type1:SubType1)") ;
    EXPECT_EQ(defaulted.str(), "Dependent(Type1:Type1)") ;
    EXPECT_EQ(loose.str(), "Dependent(Type1)") ;
    EXPECT_EQ(strictOpt.str(), "Optional(Type1:SubType1)") ;
    EXPECT_EQ(defaultedOpt.str(), "Optional(Type1:Type1)") ;
    EXPECT_EQ(looseOpt.str(), "Optional(Type1)") ;
}

TEST_F(DependentTest, TestStrCast) {
    EXPECT_EQ(static_cast<std::string>(strict), "Dependent(Type1:SubType1)") ;
    EXPECT_EQ(static_cast<std::string>(defaulted), "Dependent(Type1:Type1)") ;
    EXPECT_EQ(static_cast<std::string>(loose), "Dependent(Type1)") ;
    EXPECT_EQ(static_cast<std::string>(strictOpt), "Optional(Type1:SubType1)") ;
    EXPECT_EQ(static_cast<std::string>(defaultedOpt), "Optional(Type1:Type1)") ;
    EXPECT_EQ(static_cast<std::string>(looseOpt), "Optional(Type1)") ;
}

TEST_F(DependentTest, TestRequred) {
    EXPECT_TRUE(strict.required()) ;
    EXPECT_FALSE(strictOpt.required()) ;
}
