#pragma once

#include <string>
#include <unordered_map>

int main(int argc, const char ** argv) ;

namespace theoria { namespace util {

/**
 * Pseudo-Singleton that captures command line arguments of Theoria applications
 *
 * CommandLine consists of:
 *
 *      config file - e.g., myapp.toml
 *      settings passed as: [--setting value] or [--someFlag] 
 *      settings are terminated by optionak -- (mandatory if variables are specified)
 *      variables passed as: var1=value
 *
 *      Settings are usually predefined by theoria to change its behavior in some way specified by: theoria --help.
 *      However, user defined settings are allowed. To avoid conflict with future extensions of theoria
 *      user-defined settings should contain at least one capital letter (as theoria built-in settings are always lowere case)
 *
 *
 *      Variables are typically user defined settings that parameterize the config file. 
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

    /**
     * Constructor takes argc and argv form main() (with first arg dropped)
     * to initialize the command line object.
     * It does not validate the actual values but does check that they conform to the 
     * order expected by theoria
     */
    CommandLine(int argc, const char * argv[], bool allowMissingConfig=false) ;

    /**
     * The iterator type for settings and variables
     */
    using const_iterator = SymbolTbl::const_iterator ;

    /**
     * The instance constructed in main (or first instance)
     */
    static const CommandLine& instance() ;

    /**
     * Reset the first instance
     */
    static void reset() ; 

    /**
     * Find var if it exists
     * @return const_iterator to std::pair<var,value> else endVars()
     */
    const_iterator findVar(const std::string& var) const { return _variables.find(var) ; }

    /**
     * Test if variable exists
     * @param var the variable
     * @return true if exists
     */
    bool hasVariable(const std::string& var) const { return findVar(var) != endVars() ; }

    /**
     * Iterate over all variables
     * @return iterator to std::pair<name,value>  or end()
     */
    const_iterator beginVars() const { return _variables.cbegin(); } 

    /**
     * The end of variables iterator
     */
    const_iterator endVars() const { return _variables.cend(); }

    /**
     * Get variable value or nullptr as const char *
     * @return value or nullptr if not present
     */
    const char * variableAsPtr(const std::string& name) const noexcept ;

    /**
     * Get variable value as std::string
     * @return value or def if not present
     */
    const std::string& variableAsStr(const std::string& name, const std::string& def = "") const ;

    /**
     * Get variable value as int64_t
     * @return value or def if not present
     * @throw std::runtime_error if can't convert to an integer
     */
    int64_t variableAsInt(const std::string& name, int64_t def=0) const ;


    /**
     * Get variable value as double
     * @return value or def if not present
     * @throw std::runtime_error if can't convert to a double
     */
    double variableAsDbl(const std::string& name, double def = 0) const ;

    /**
     * Get variable value as bool
     * @return value or def if not present
     * @throw std::runtime_error if can't convert to a bool 
     */
    bool variableAsBool(const std::string& name, bool def) const ;

    /**
     * Return the number of variables
     */
    int numVars() const {return _variables.size();} 
   
    /**
     * Iterator to the named setting if it exists otherwise endSettings()
     */
    const_iterator findSetting(const std::string& name) const { return _settings.find(name) ; }
   
    /**
     * True if a setting with specified name exists
     */
    bool hasSetting(const std::string& name) const { return findSetting(name) != endSettings() ; }
   
    /**
     * Iterator to the first setting
     */
    const_iterator beginSettings() const {return _settings.cbegin(); }
   
    /**
     * End of settings
     */
    const_iterator endSettings() const {return _settings.cend(); }
   
    /**
     * Return a settting as a const char * if it exists otherwise return nullptr
     */
    const char * settingAsPtr(const std::string& name) const noexcept ;
   
    /**
     * Return a settting as a string if it exists otherwise return def
     */
    const std::string& settingAsStr(const std::string& name, const std::string& def) const ;
   
    /**
     * Return a settting as a 64bit integer if it exists otherwise return def
     */
    int64_t settingAsInt(const std::string& name, int64_t def) const ;
   
    /**
     * Return a setting as double if it exists otherwise return def
     */
    double settingAsDbl(const std::string& name, double def) const ;
   
    /**
     * Return a setting as a bool if it exists otherwise return def
     */
    bool settingAsBool(const std::string& name, bool def) const ;
   
    /**
     * The numbber of settings passed on the command line
     */
    int numSettings() const {return _settings.size();} 

    /**
     * The name of the app config file bassed on the command line
     */
    const std::string& configFilename() const {return _configFileName;}

private:


    CommandLine() ; 
    CommandLine(const CommandLine&) ;
    CommandLine& operator =(const CommandLine&) ;


    SymbolTbl _settings ;
    SymbolTbl _variables ;
    std::string _configFileName ;
} ;


}} //-namespace theoria::util 
