#include <theoria/core/Bootstrap.h>

#include <theoria/envvars.h>
#include <theoria/options.h>

#include <theoria/config/Config.h>
#include <theoria/config/TOMLConfigBuilder.h>
#include <theoria/except/except.h>
#include <theoria/os/os.h>
#include <theoria/util/CommandLine.h>


#include <cstdlib>
#include <string>

using namespace theoria ;
using namespace core ;

std::unique_ptr<config::Config> Bootstrap::loadConfig() const
{
    std::string filename = findConfig() ;
    config::TOMLConfigBuilder builder ;
    return std::unique_ptr<config::Config>(builder.parse_file(filename)) ;
}

std::string Bootstrap::findConfig() const
{
    std::string path = util::CommandLine::instance().settingAsStr(OPTION_BOOTSTRAP_CONFIG_PATH, ::getenv(ENV_BOOTSTRAP_CONFIG_PATH)) ; 
    if (path == "")
        path = os::bootstrap_path() ;

    auto dirs = os::split_search_path(path) ;
    std::string filename ;
    for (auto dir : dirs) {
        filename = os::join_path(dir, "bootstrap.toml") ;
        if (os::exists(filename)) 
            break ;
    }
    if (filename == "")
        throw RUNTIME_ERROR("boostrap.toml not found whils searching %s. Can't proceed.", path.c_str()) ; 
    return filename ;
}

void Bootstrap::boot()
{
    //TODO: shake Theoria's booty
}

