#pragma once

#include <string>
#include <unordered_map>

int main(int argc, const char ** argv) ;

namespace theoria { namespace util {

/*
 * Singleton that captures command line arguemenst of theoria applications
 *
 * CommandLine consists of:
 *
 *      config file - e.g., myapp.toml
 *      settings passed as: [--setting value] or [--someFlag] 
 *      settings are terminated by optionak -- (mandatory if variables are specified)
 *      variables passed as: var1=value
 *
 *      Settings are usually predfined by theoria to change its behavior in some way specified by: theoria --help.
 *      However, user defined settings are allowed. To avoid conflict with future extensions of theoria
 *      userdefined settings should conatin at least one capital letter (as theoria built-in settings are always lowere case)
 *
 *
 *      Variables are typically user defined settings that paramterize the config file. 
 *      See <theoria::config::CmdLineResolver>
 *
 * Example legal command lines:
 *
 *      theoria config.toml
 *      theoria config.xml
 *      theoria config.toml --setting1
 *      theoria config.toml --setting1 97
 *      theoria config.toml --setting1 --setting2 17
 *      theoria config.toml --setting1 --setting2 17 --
 *      theoria config.toml -- a=1 b=2 c=3
 *      theoria config.toml --setting1 --setting2 17 -- a=1 b=2 c=3 d=[1,2,3,4]
 */


class CommandLine {

    using SymbolTbl = std::unordered_map<std::string, std::string> ;

public:

    using const_iterator = SymbolTbl::const_iterator ;

    static const CommandLine& instance() ;

    /* Test if variable exists
     * @var the variable
     * @return true if exists
     */
    bool hasVariable(const std::string& var) const ;

    /*
     * Iterate over all variables
     * @return iterator to std::pair<name,value>  or end()
     */
    const_iterator beginVars() const ; 
    const_iterator endVars() const ; 

    /*
     * Get variable value or nullptr as const char *
     * @return value or nullptr if not present
     */
    const char * variableAsPtr(const std::string& name) const noexcept ;

    /*
     * Get variable value as std::string
     * @return value or def if not present
     */
    const std::string& variableAsStr(const std::string& name, const std::string& def = "") const ;

    /*
     * Get variable value as int64_t
     * @return value or def if not present
     */
    int64_t variableAsInt(const std::string& name, int64_t def=0) const ;


    /*
     * Get variable value as double
     * @return value or def if not present
     */
    double variableAsDbl(const std::string& name, double def = 0) const ;

    bool variableAsBool(const std::string& name, bool def) const ;

    
    bool hasSetting(const std::string& name) const ;
    const_iterator beginSettings() const ; 
    const_iterator endSettings() const ; 
    const char * settingAsPtr(const std::string& name) const noexcept ;
    const std::string& settingAsStr(const std::string& name, const std::string& def) const ;
    int64_t settingAsInt(const std::string& name, int64_t def) const ;
    double settingAsDbl(const std::string& name, double def) const ;
    bool settingAsBool(const std::string& name, bool def) const ;


private:

    friend int ::main(int argc, const char ** argv) ;

    CommandLine() ; 
    CommandLine(const CommandLine&) ;
    CommandLine& operator =(const CommandLine&) ;

    CommandLine(int argc, const char * argv[]) ;

    SymbolTbl _settings ;
    SymbolTbl _variables ;
    std::string _configFileName ;
} ;


}} //-namespace theoria::util 
