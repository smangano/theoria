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
        const ConfigVariableResolver* next = _resolver ;
        while(next) {
            auto temp = next ;
            next = next->next() ;
            delete temp ;
        }

    }

} ; 

TEST_F(EnvVarResolveTest, Name) {
    EXPECT_EQ(resolver().name(), "EnvVarResolver");
}
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
        const ConfigVariableResolver* next = _resolver ;
        while(next) {
            auto temp = next ;
            next = next->next() ;
            delete temp ;
        }
        _cl.reset() ;
        CommandLine::reset() ;
    }

private:
    std::unique_ptr<CommandLine> _cl ;
};


TEST_F(CmdLineResolverTest, Name) {
    EXPECT_EQ(resolver().name(), "CmdLineResolver");
}

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
        const ConfigVariableResolver* next = _resolver ;
        while(next) {
            auto temp = next ;
            next = next->next() ;
            delete temp ;
        }
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
    auto disallow = new DisallowResolver() ;
    EXPECT_EQ(disallow->name(), "DisallowResolver") ;
    resolver().append(disallow) ;
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
    auto disable = new DisableResolver() ;
    EXPECT_EQ(disable->name(), "DisableResolver") ;
    resolver().append(disable) ;
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

static const char * TOML = \
R"(

    name1 = 10 
    name2 = 20 
    name3 = 3.1415
    name4 = true
    dateTime = 2000-01-01T00:00:00.000100Z
    [table1]
    name1 = 11
    name2 = 21
    name3 = "3.1415"
    name4 = false
    endofworld = 2016-11-08T23:50:50-05:00
)" ;

class TOMLResolverTest : public ResolveTestBase
{
    virtual void SetUp() {

        std::istringstream iss(TOML) ;
        _resolver = new TOMLResolver(iss) ; 
    }

    virtual void TearDown() {
        delete _resolver ;
    }
};

TEST_F(TOMLResolverTest, Name) {
    EXPECT_EQ(_resolver->name() , "TOMLResolver") ;
}

TEST_F(TOMLResolverTest, ResolveFirst) {
    EXPECT_EQ(_resolver->resolve("$name1") , "10") ;
    EXPECT_EQ(_resolver->resolve("$name2") , "20") ;
    EXPECT_EQ(_resolver->resolve("$name3") , "3.1415") ;
    EXPECT_EQ(_resolver->resolve("$name4") , "true") ;
    EXPECT_EQ(_resolver->resolve("$dateTime") , "2000-01-01T00:00:00.000100") ;
    EXPECT_EQ(_resolver->resolve("$table1.name1") , "11") ;
    EXPECT_EQ(_resolver->resolve("$table1.name2") , "21") ;
    EXPECT_EQ(_resolver->resolve("$table1.name3") , "3.1415") ;
    EXPECT_EQ(_resolver->resolve("$table1.name4") , "false") ;
    EXPECT_EQ(_resolver->resolve("$table1.endofworld") , "2016-11-08T23:50:50-05:00") ;
    EXPECT_THROW(_resolver->resolve("$"), std::runtime_error) ;
    EXPECT_THROW(_resolver->resolve(""), std::runtime_error) ;
    EXPECT_THROW(_resolver->resolve("_"), std::runtime_error) ;
    EXPECT_THROW(_resolver->resolve("name1"), std::runtime_error) ;
}

TEST_F(TOMLResolverTest, BadTOML) {
    std::istringstream iss("blah-1") ;
    EXPECT_THROW(TOMLResolver {iss}, std::runtime_error) ; 
}

TEST_F(TOMLResolverTest, BadTOMLFile) {
    EXPECT_THROW( TOMLResolver("blah.toml"), std::runtime_error) ; 
}
