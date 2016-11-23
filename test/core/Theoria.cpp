#include <theoria/core/Theoria.h>
#include <gtest/gtest.h>
#include <sstream>

using namespace theoria ;
using namespace core ;

static const char * EXPECTED_HELP = \
R"(usage: theoria [--bootstrap_pathpath] [--default_log_levellevel] [--help[option]] 
               [--init_only] [--pause_after_app_init] [--pause_after_app_finalize] 
               [--show_config] [--show_config_only] [--ws_portport] 
               appconfigName [-- [user settings]]

bootstrap_path <path>
Path to bootstrap.ini
    Example: --bootstrap_path /home/jsmith/theoria/boot.toml

default_log_level <level>
Set the default logging level
    Example: --default_log_level DEBUG
    Choices: DEBUG,INFO, WARN, ERROR, CRITICAL

help <[option]>
Display help for theoria or a particular option
    Example: --help or -help option

init_only 
Used to test changes to bootstrap.toml

pause_after_app_init 
Initializes all app components and pauses waiting for operator interaction

pause_after_app_finalize 
Finalizes all app components and pauses waiting for operator interaction

show_config 
Displays config file after variable resolution

show_config_only 
Exits after displaying config file after variable resolution

ws_port <port>
Start web service for interactivity and use this port
    Example: -ws_port 8900

)" ;

TEST(TheoriaTest, Help) {
    std::ostringstream oss ;
    Theoria t ;
    t.help(oss) ;
    EXPECT_EQ(oss.str(), EXPECTED_HELP) ;
}

TEST(TheoriaTest, HelpOption) {
    std::ostringstream oss ;
    auto expected = \
    R"(help <[option]>
Display help for theoria or a particular option
    Example: --help or -help option

)" ;

    Theoria t ;
    t.help("help", oss) ;
    EXPECT_EQ(oss.str(), expected) ;
}

TEST(TheoriaTest, HelpOptionBad) {
    std::ostringstream oss ;
    auto expected = "'blah' is not a valid theoria option\n" ;
    Theoria t ;
    t.help("blah", oss) ;
    EXPECT_EQ(oss.str(), expected) ;
}
