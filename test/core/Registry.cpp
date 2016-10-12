#include "theoria/core/Registry.h"
#include <gtest/gtest.h>

using namespace theoria ;
using namespace theoria::core ;

class RegistryTest : public ::testing::Test 
{
    virtual void TearDown() {
        Registry::reset() ;
    } 
} ;


TEST_F(RegistryTest, RegisterFactoryByTypenameOnly) {
}

TEST_F(RegistryTest, RegisterFactoryByTypenameAndSubType) {
}
