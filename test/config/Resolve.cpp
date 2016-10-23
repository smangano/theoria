#include "theoria/config/Resolve.h"
#include "theoria/util/CommandLine.h"
#include <gtest/gtest.h>
#include <cstdlib>

using namespace theoria ;
using namespace config ;
using namespace util ;

class ResolveTestBase : public ::testing::Test
{
protected:

   ConfigVariableResolver& resolver() {return *_resolver;} 

   ConfigVariableResolver*  _resolver ;
} ;

class EnvVarResolveTest  : public ResolveTestBase
{
public:

    virtual void SetUp() {
       _resolver = new EnvVarResolver() ;  
       putenv((char*)"EnvVarResolveTest_1=1");
       putenv((char*)"EnvVarResolveTest_2=Foo");

    }

    virtual void TearDown() {
        delete _resolver ;

    }

} ; 

TEST_F(EnvVarResolveTest, ResolveFirstTest) {
    EXPECT_EQ(std::string(getenv("EnvVarResolveTest_1")), std::string("1")) ;
    EXPECT_EQ(resolver().resolve("$EnvVarResolveTest_1"), std::string(getenv("EnvVarResolveTest_1")));
    EXPECT_EQ(resolver().resolve("$EnvVarResolveTest_2"), std::string(getenv("EnvVarResolveTest_2")));
}

TEST_F(EnvVarResolveTest, ResolveLastTest) {
    EXPECT_EQ(std::string(getenv("EnvVarResolveTest_1")), std::string("1")) ;
    EXPECT_EQ(resolver().resolve("$$EnvVarResolveTest_1"), getenv("EnvVarResolveTest_1"));
    EXPECT_EQ(resolver().resolve("$$EnvVarResolveTest_2"), getenv("EnvVarResolveTest_2"));
}

class CmdLineResolverTest : public ResolveTestBase
{

    virtual void SetUp() {
        bool allowMissingConfig = true ;
        const char * argv[] = {"dummy", "--", "test1=1", "testFoo=foo"} ;
        _cl.reset(new CommandLine(4, argv, allowMissingConfig )) ;
        _resolver = new CmdLineResolver() ; 
    }

    virtual void TearDown() {
        delete _resolver ;
        _cl.reset() ;
        CommandLine::reset() ;
    }

private:
    std::unique_ptr<CommandLine> _cl ;
};


TEST_F(CmdLineResolverTest, ResolveFirstTest) {
    EXPECT_EQ(resolver().resolve("$test1"), "1");
    EXPECT_EQ(resolver().resolve("$testFoo"), "foo");
}

TEST_F(CmdLineResolverTest, ResolveLastTest) {
    EXPECT_EQ(resolver().resolve("$$test1"), "1");
    EXPECT_EQ(resolver().resolve("$$testFoo"), "foo");
}

class ChainedResolverTest : public ResolveTestBase
{
    virtual void SetUp() {
        putenv((char*)"EnvVarResolveTest_1=1");
        putenv((char*)"EnvVarResolveTest_2=Foo");
        putenv((char*)"testFoo=NotFoo");
        bool allowMissingConfig = true ;
        const char * argv[] = {"dummy", "--", "test1=1", "testFoo=foo"} ;
        _cl.reset(new CommandLine(4, argv, allowMissingConfig )) ;
        _resolver = new CmdLineResolver() ; 
        _resolver->append(new EnvVarResolver()) ;
    }

    virtual void TearDown() {
        delete _resolver ;
        _cl.reset() ;
        CommandLine::reset() ;
    }

private:
    std::unique_ptr<CommandLine> _cl ;
};

TEST_F(ChainedResolverTest, ResolveFirstTest) {
    EXPECT_EQ(resolver().resolve("$EnvVarResolveTest_1"), "1");
    EXPECT_EQ(resolver().resolve("$EnvVarResolveTest_2"), "Foo");
    EXPECT_EQ(resolver().resolve("$test1"), "1");
    //This is key because testFoo is in both resolvers but CmdLineResolver is first in the chain
    EXPECT_EQ(resolver().resolve("$testFoo"), "foo");
}

TEST_F(ChainedResolverTest, ResolveLastTest) {
    EXPECT_EQ(resolver().resolve("$$EnvVarResolveTest_1"), "1");
    EXPECT_EQ(resolver().resolve("$$EnvVarResolveTest_2"), "Foo");
    EXPECT_EQ(resolver().resolve("$$test1"), "1");
    //This is key because testFoo is in both resolvers but EnvVarResolver is last in the chain
    EXPECT_EQ(resolver().resolve("$$testFoo"), "NotFoo");
}

TEST_F(ChainedResolverTest, ResolveFirstTestWithDissallow) {
    resolver().append(new DisallowResolver()) ;
    EXPECT_EQ(resolver().resolve("$EnvVarResolveTest_1"), "1");
    EXPECT_EQ(resolver().resolve("$EnvVarResolveTest_2"), "Foo");
    EXPECT_EQ(resolver().resolve("$test1"), "1");
    //This is key because testFoo is in both resolvers but CmdLineResolver is first in the chain
    EXPECT_EQ(resolver().resolve("$testFoo"), "foo");
}

TEST_F(ChainedResolverTest, ResolveLastTestWithDisallow) {
    resolver().append(new DisallowResolver()) ;
    EXPECT_THROW(resolver().resolve("$$EnvVarResolveTest_1"), std::runtime_error);
    EXPECT_THROW(resolver().resolve("$$testFoo"), std::runtime_error);
}

TEST_F(ChainedResolverTest, ResolveFirstTestWithDisable) {
    resolver().append(new DisableResolver()) ;
    EXPECT_EQ(resolver().resolve("$EnvVarResolveTest_1"), "1");
    EXPECT_EQ(resolver().resolve("$EnvVarResolveTest_2"), "Foo");
    EXPECT_EQ(resolver().resolve("$test1"), "1");
    //This is key because testFoo is in both resolvers but CmdLineResolver is first in the chain
    EXPECT_EQ(resolver().resolve("$testFoo"), "foo");
}

TEST_F(ChainedResolverTest, ResolveLastTestWithDisable) {
    resolver().append(new DisableResolver()) ;
    EXPECT_EQ(resolver().resolve("$$EnvVarResolveTest_1"), "EnvVarResolveTest_1");
    EXPECT_EQ(resolver().resolve("$$testFoo"), "testFoo");
}
