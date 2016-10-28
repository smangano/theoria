#include "theoria/util/strings.h"
#include <gtest/gtest.h>


TEST(StringSplitTest, EmptyTest) {
   EXPECT_EQ(theoria::util::string_split("").size(), 0)  ;
}

TEST(StringSplitTest, OneElemTest) {

    auto result = theoria::util::string_split("hello") ;
    ASSERT_EQ(result.size(), 1)  ;
    EXPECT_EQ(result[0], "hello") ;
}

TEST(StringSplitTest, TrailingDelim) {

    auto result = theoria::util::string_split("hello ") ;
    ASSERT_EQ(result.size(), 1)  ;
    EXPECT_EQ(result[0], "hello") ;
}

TEST(StringSplitTest, LeadingDelim) {

    auto result = theoria::util::string_split(" hello") ;
    ASSERT_EQ(result.size(), 2)  ;
    EXPECT_EQ(result[0], "") ;
    EXPECT_EQ(result[1], "hello") ;
}

TEST(StringSplitTest, TwoElem) {

    auto result = theoria::util::string_split("hello,world", ',') ;
    ASSERT_EQ(result.size(), 2)  ;
    EXPECT_EQ(result[0], "hello") ;
    EXPECT_EQ(result[1], "world") ;
}
