#include "theoria/util/CommandLine.h"
#include <gtest/gtest.h>

#include <stdexcept>

using namespace theoria ;
using namespace theoria::util ;

class CommandLineTest : public ::testing::Test 
{
    virtual void SetUp() {
       tmpnam(config_file_name) ;
       config_file = fopen(config_file_name, "w+") ; 
    }

    virtual void TearDown() {
        CommandLine::reset() ;
        fclose(config_file) ;
        remove(config_file_name) ;
    }

protected:

    char  config_file_name[L_tmpnam] ;
    FILE* config_file ;
    
} ;

TEST_F(CommandLineTest, NoArgsThrows) {
   ASSERT_THROW(CommandLine(0, nullptr), std::runtime_error) ;
}

TEST_F(CommandLineTest, ConfigFileOnly) {
    const char * args[1] ;
    args[0] = config_file_name ;
    CommandLine cl(1, args) ;
    ASSERT_EQ(&CommandLine::instance(), &cl) ;
    ASSERT_EQ(CommandLine::instance().configFilename(), config_file_name) ;
    ASSERT_EQ(CommandLine::instance().beginVars(), CommandLine::instance().endVars()) ;
    ASSERT_EQ(CommandLine::instance().beginSettings(), CommandLine::instance().beginSettings()) ;
}

TEST_F(CommandLineTest, ConfigFileOnlyDashDash) {
    const char * args[2] ;
    args[0] = config_file_name ;
    args[1] = "--" ;
    CommandLine cl(2, args) ;
    ASSERT_EQ(&CommandLine::instance(), &cl) ;
    ASSERT_EQ(CommandLine::instance().configFilename(), config_file_name) ;
    ASSERT_EQ(CommandLine::instance().beginVars(), CommandLine::instance().endVars()) ;
    ASSERT_EQ(CommandLine::instance().beginSettings(), CommandLine::instance().endSettings()) ;
}

TEST_F(CommandLineTest, ConfigFileAndOneBareSetting) {
    const char * args[2] ;
    args[0] = config_file_name ;
    args[1] = "--foo" ;
    CommandLine cl(2, args) ;
    ASSERT_EQ(&CommandLine::instance(), &cl) ;
    ASSERT_EQ(CommandLine::instance().configFilename(), config_file_name) ;
    ASSERT_EQ(CommandLine::instance().beginVars(), CommandLine::instance().endVars()) ;
    ASSERT_TRUE(CommandLine::instance().hasSetting("foo")) ;
}

TEST_F(CommandLineTest, ConfigFileAndOneSetting) {
    const char * args[3] ;
    args[0] = config_file_name ;
    args[1] = "--foo" ;
    args[2] = "10" ;
    CommandLine cl(3, args) ;
    ASSERT_EQ(&CommandLine::instance(), &cl) ;
    ASSERT_EQ(CommandLine::instance().configFilename(), config_file_name) ;
    ASSERT_EQ(CommandLine::instance().beginVars(), CommandLine::instance().endVars()) ;
    ASSERT_TRUE(CommandLine::instance().hasSetting("foo")) ;
    ASSERT_EQ(CommandLine::instance().settingAsInt("foo", 0), 10) ;
}

TEST_F(CommandLineTest, ConfigFileAndOneSettingDashDash) {
    const char * args[4] ;
    args[0] = config_file_name ;
    args[1] = "--foo" ;
    args[2] = "10" ;
    args[3] = "--" ;
    CommandLine cl(4, args) ;
    ASSERT_EQ(&CommandLine::instance(), &cl) ;
    ASSERT_EQ(CommandLine::instance().configFilename(), config_file_name) ;
    ASSERT_EQ(CommandLine::instance().beginVars(), CommandLine::instance().endVars()) ;
    ASSERT_TRUE(CommandLine::instance().hasSetting("foo")) ;
    ASSERT_EQ(CommandLine::instance().settingAsInt("foo", 0), 10) ;
    ASSERT_EQ(CommandLine::instance().numSettings(), 1) ;
}

TEST_F(CommandLineTest, ConfigFileAndMixedSettings) {
    const char * args[6] ;
    args[0] = config_file_name ;
    args[1] = "--foo" ;
    args[2] = "10" ;
    args[3] = "--bar" ;
    args[4] = "--baz" ;
    args[5] = "3.14" ;
    CommandLine cl(6, args) ;
    ASSERT_EQ(&CommandLine::instance(), &cl) ;
    ASSERT_EQ(CommandLine::instance().configFilename(), config_file_name) ;
    ASSERT_EQ(CommandLine::instance().beginVars(), CommandLine::instance().endVars()) ;
    ASSERT_TRUE(CommandLine::instance().hasSetting("foo")) ;
    ASSERT_EQ(CommandLine::instance().settingAsInt("foo", 0), 10) ;
    ASSERT_EQ(CommandLine::instance().settingAsBool("bar", false), true) ;
    ASSERT_EQ(CommandLine::instance().settingAsDbl("baz", 0), 3.14) ;
    ASSERT_EQ(CommandLine::instance().numSettings(), 3) ;
}

TEST_F(CommandLineTest, ConfigFileAndMixedSettingsDashDash) {
    const char * args[7] ;
    args[0] = config_file_name ;
    args[1] = "--foo" ;
    args[2] = "10" ;
    args[3] = "--bar" ;
    args[4] = "--baz" ;
    args[5] = "3.14" ;
    args[6] = "--" ;
    CommandLine cl(7, args) ;
    ASSERT_EQ(&CommandLine::instance(), &cl) ;
    ASSERT_EQ(CommandLine::instance().configFilename(), config_file_name) ;
    ASSERT_EQ(CommandLine::instance().beginVars(), CommandLine::instance().endVars()) ;
    ASSERT_TRUE(CommandLine::instance().hasSetting("foo")) ;
    ASSERT_EQ(CommandLine::instance().settingAsInt("foo", 0), 10) ;
    ASSERT_EQ(CommandLine::instance().settingAsBool("bar", false), true) ;
    ASSERT_EQ(CommandLine::instance().settingAsDbl("baz", 0), 3.14) ;
    ASSERT_EQ(CommandLine::instance().numSettings(), 3) ;
}

TEST_F(CommandLineTest, ConfigFileNoSettingsOneVar) {
    const char * args[3] ;
    args[0] = config_file_name ;
    args[1] = "--" ;
    args[2] = "a=10" ;
    CommandLine cl(3, args) ;
    ASSERT_EQ(&CommandLine::instance(), &cl) ;
    ASSERT_EQ(CommandLine::instance().configFilename(), config_file_name) ;
    ASSERT_EQ(CommandLine::instance().beginSettings(), CommandLine::instance().endSettings()) ;
    ASSERT_TRUE(CommandLine::instance().hasVariable("a")) ;
    ASSERT_EQ(CommandLine::instance().variableAsInt("a", 0), 10) ;
}

TEST_F(CommandLineTest, ConfigFileNoSettingsMixedVars) {
    const char * args[5] ;
    args[0] = config_file_name ;
    args[1] = "--" ;
    args[2] = "a=10" ;
    args[3] = "b=3.14" ;
    args[4] = "c=hello" ;
    CommandLine cl(5, args) ;
    ASSERT_EQ(&CommandLine::instance(), &cl) ;
    ASSERT_EQ(CommandLine::instance().configFilename(), config_file_name) ;
    ASSERT_EQ(CommandLine::instance().beginSettings(), CommandLine::instance().endSettings()) ;
    ASSERT_EQ(CommandLine::instance().numVars(), 3) ;
    ASSERT_EQ(CommandLine::instance().variableAsInt("a", 0), 10) ;
    ASSERT_EQ(CommandLine::instance().variableAsDbl("b", 0), 3.14) ;
    ASSERT_EQ(CommandLine::instance().variableAsStr("c", ""), "hello") ;
}

TEST_F(CommandLineTest, ConfigFileAndMixedSettingsAndVariables) {
    const char * args[10] ;
    args[0] = config_file_name ;
    args[1] = "--foo" ;
    args[2] = "10" ;
    args[3] = "--bar" ;
    args[4] = "--baz" ;
    args[5] = "3.14" ;
    args[6] = "--" ;
    args[7] = "a=10" ;
    args[8] = "b=3.14" ;
    args[9] = "c=hello" ;
    CommandLine cl(10, args) ;
    ASSERT_EQ(&CommandLine::instance(), &cl) ;
    ASSERT_EQ(CommandLine::instance().numSettings(), 3) ;
    ASSERT_EQ(CommandLine::instance().configFilename(), config_file_name) ;
    ASSERT_EQ(CommandLine::instance().settingAsInt("foo", 0), 10) ;
    ASSERT_EQ(CommandLine::instance().settingAsBool("bar", false), true) ;
    ASSERT_EQ(CommandLine::instance().settingAsDbl("baz", 0), 3.14) ;
    ASSERT_EQ(CommandLine::instance().numVars(), 3) ;
    ASSERT_EQ(CommandLine::instance().variableAsInt("a", 0), 10) ;
    ASSERT_EQ(CommandLine::instance().variableAsDbl("b", 0), 3.14) ;
    ASSERT_EQ(CommandLine::instance().variableAsStr("c", ""), "hello") ;
}

TEST_F(CommandLineTest, SettingAsPtr) {
    const char * args[3] ;
    args[0] = config_file_name ;
    args[1] = "--foo" ;
    args[2] = "test" ;
    CommandLine cl(3, args) ;
    ASSERT_TRUE(CommandLine::instance().settingAsPtr("foo") != nullptr) ;
    ASSERT_EQ(std::string(CommandLine::instance().settingAsPtr("foo")),  "test") ;
    ASSERT_TRUE(CommandLine::instance().settingAsPtr("bar") == nullptr) ;
}

TEST_F(CommandLineTest, SettingAsStr) {
    const char * args[3] ;
    args[0] = config_file_name ;
    args[1] = "--foo" ;
    args[2] = "test" ;
    CommandLine cl(3, args) ;
    ASSERT_EQ(CommandLine::instance().settingAsStr("foo", ""),  "test") ;
    ASSERT_EQ(CommandLine::instance().settingAsStr("bar", "default"),  "default") ;
}

TEST_F(CommandLineTest, SettingAsInt) {
    const char * args[4] ;
    args[0] = config_file_name ;
    args[1] = "--foo" ;
    args[2] = "9999999" ;
    args[3] = "--baz" ;
    CommandLine cl(4, args) ;
    ASSERT_EQ(CommandLine::instance().settingAsInt("foo", 0), 9999999) ;
    ASSERT_EQ(CommandLine::instance().settingAsInt("bar", 27),  27) ;
    ASSERT_THROW(CommandLine::instance().settingAsInt("baz", 27),  std::runtime_error) ;
}

TEST_F(CommandLineTest, SettingAsDbl) {
    const char * args[4] ;
    args[0] = config_file_name ;
    args[1] = "--foo" ;
    args[2] = "9.9e10" ;
    args[3] = "--baz" ;
    CommandLine cl(4, args) ;
    ASSERT_EQ(CommandLine::instance().settingAsDbl("foo", 0), 9.9e10) ;
    ASSERT_EQ(CommandLine::instance().settingAsDbl("bar", 2.7),  2.7) ;
    ASSERT_THROW(CommandLine::instance().settingAsDbl("baz", 2.7),  std::runtime_error) ;
}

TEST_F(CommandLineTest, SettingAsBool) {
    const char * args[11] ;
    args[0] = config_file_name ;
    args[1] = "--foo" ;
    args[2] = "true" ;
    args[3] = "--bar" ;
    args[4] = "false" ;
    args[5] = "--baz" ;
    args[6] = "1" ;
    args[7] = "--bing" ;
    args[8] = "0" ;
    args[9] = "--bam" ;
    args[10] = "nada" ;
    CommandLine cl(11, args) ;
    ASSERT_EQ(CommandLine::instance().settingAsBool("foo", false), true) ;
    ASSERT_EQ(CommandLine::instance().settingAsBool("bar", true),  false) ;
    ASSERT_EQ(CommandLine::instance().settingAsBool("baz", false), true) ;
    ASSERT_EQ(CommandLine::instance().settingAsBool("bing", true),  false) ;
    ASSERT_THROW(CommandLine::instance().settingAsBool("bam", false),  std::runtime_error) ;
}
