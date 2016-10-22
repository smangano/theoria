#include "theoria/config/Resolve.h"
#include <gtest/gtest.h>
#include <cstdlib>


class EnvVarResolveTest  : public ::testing::Test 
{
    virtual void SetUp() {
       _resolver = new EnvVarResolver() ;  
       putenv("EnvVarResolveTest_1=1");
       putenv("EnvVarResolveTest_2=Foo");

    }

    virtual void TearDown() {
        delete _resolver ;

    }

   ConfigVariableResolver*  _resolver ;
} ; 

TEST_F(EnvVarResolveTest, ResolveFirstTest) {
    ASSERT_EQ(getenv("EnvVarResolveTest_1"), "1") ;
    ASSERT_EQ(_resolver->resolve("$EnvVarResolveTest_1"), getenv("EnvVarResolveTest_1"));
    ASSERT_EQ(_resolver->resolve("$EnvVarResolveTest_2"), getenv("EnvVarResolveTest_2"));
}

TEST_F(EnvVarResolveTest, ResolveLastTest) {
    ASSERT_EQ(getenv("EnvVarResolveTest_1"), "1") ;
    ASSERT_EQ(_resolver->resolve("$$EnvVarResolveTest_1"), getenv("EnvVarResolveTest_1"));
    ASSERT_EQ(_resolver->resolve("$$EnvVarResolveTest_2"), getenv("EnvVarResolveTest_2"));
}

class CmdLineResolverTest : public ::testing::Test 
{

    virtual void SetUp() {
        CommandLine(4, "dummy", "--", "test1=1", "testFoo=foo", allowMissingConfig ) ;
         _resolver = new CmdLineResolver() ; 
        bool allowMissingConfig = true ;
    }

    virtual void TearDown() {
        delete _resolver ;
        CommandLine::reset() ;
    }

};

TEST_F(CmdLineResolverTest, ResolveFirstTest) {
    ASSERT_EQ(_resolver->resolve("$test1"), "1");
    ASSERT_EQ(_resolver->resolve("$testFoo"), "foo");
}

TEST_F(CmdLineResolverTest, ResolveLastTest) {
    ASSERT_EQ(_resolver->resolve("$$test1"), "1");
    ASSERT_EQ(_resolver->resolve("$$testFoo"), "foo");
}

