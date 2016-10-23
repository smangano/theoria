#include <theoria/util/Maybe.h>
#include <gtest/gtest.h>

using namespace theoria ;
using namespace util ;

TEST(MaybeTest, TestNull) {
    int *p = nullptr ;
    Maybe<int> maybnot(p) ;
    EXPECT_FALSE(maybnot) ;
    EXPECT_THROW(*maybnot, std::runtime_error) ;
}

TEST(MaybeTest, TestOk) {
    
    int x = 10 ;
    Maybe<int> maybeyes(&x) ;
    EXPECT_TRUE(maybeyes) ;
    EXPECT_EQ(*maybeyes, x) ;
}

namespace {
class Base
{
public:

    virtual ~Base() {} ;
} ;

class Derived : public Base
{
public:

    virtual ~Derived() {} ;
    
} ;

class Other : public Base
{
public:

    virtual ~Other() {} ;
} ;

}

TEST(MaybeTest, TestNotDerived) {
    Other other ;
    Maybe<Derived> notderived(&other) ;
    EXPECT_FALSE(notderived) ;
    EXPECT_THROW(*notderived, std::runtime_error) ;
}

TEST(MaybeTest, TestDerived) {
    Derived derived ;
    Maybe<Derived> yesderived(&derived) ;
    EXPECT_TRUE(yesderived) ;
    EXPECT_EQ(&(*yesderived), &derived) ;
}

TEST(MaybeTest, TestWhere) {
    int *p = nullptr ;
    Maybe<int> maybnot(p, "MaybeTest_TestWhere") ;
    EXPECT_FALSE(maybnot) ;
    EXPECT_STREQ(maybnot.where(), "MaybeTest_TestWhere") ;
    EXPECT_THROW(*maybnot, std::runtime_error) ;
    try {
        *maybnot ;
    } 
    catch (std::runtime_error& err) 
    {
        EXPECT_NE(strstr(err.what(), "Maybe not! Object was not bound at [MaybeTest_TestWhere]"), nullptr) ;
    }
}

TEST(MaybeTest, TestValueOr) {
    int *p = nullptr ;
    Maybe<int> maybnot(p) ;
    int x = 10 ;
    Maybe<int> maybeyes(&x) ;

    EXPECT_EQ(maybnot.valueOr(11), 11) ;
    EXPECT_EQ(maybeyes.valueOr(11), 10) ;
}


TEST(MaybeTest, TestNotAsDerived) {
    Other other ;
    Maybe<Base> notderived(static_cast<Base*>(&other)) ;
    EXPECT_THROW(notderived.as<Derived>(), std::bad_cast) ;
}

TEST(MaybeTest, TestAsDerived) {
    Derived derived ;
    Maybe<Base> yesderived(static_cast<Base*>(&derived)) ;
    EXPECT_EQ(&yesderived.as<Derived>(), &derived) ;
}
