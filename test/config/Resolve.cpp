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
    ASSERT_EQ(std::string(getenv("EnvVarResolveTest_1")), std::string("1")) ;
    ASSERT_EQ(resolver().resolve("$EnvVarResolveTest_1"), std::string(getenv("EnvVarResolveTest_1")));
    ASSERT_EQ(resolver().resolve("$EnvVarResolveTest_2"), std::string(getenv("EnvVarResolveTest_2")));
}

TEST_F(EnvVarResolveTest, ResolveLastTest) {
    ASSERT_EQ(std::string(getenv("EnvVarResolveTest_1")), std::string("1")) ;
    ASSERT_EQ(resolver().resolve("$$EnvVarResolveTest_1"), getenv("EnvVarResolveTest_1"));
    ASSERT_EQ(resolver().resolve("$$EnvVarResolveTest_2"), getenv("EnvVarResolveTest_2"));
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
        _cl.release() ;
        CommandLine::reset() ;
    }

private:
    std::unique_ptr<CommandLine> _cl ;
};

TEST_F(CmdLineResolverTest, ResolveFirstTest) {
    ASSERT_EQ(resolver().resolve("$test1"), "1");
    ASSERT_EQ(resolver().resolve("$testFoo"), "foo");
}

TEST_F(CmdLineResolverTest, ResolveLastTest) {
    ASSERT_EQ(resolver().resolve("$$test1"), "1");
    ASSERT_EQ(resolver().resolve("$$testFoo"), "foo");
}

