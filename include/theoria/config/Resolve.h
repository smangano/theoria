#pragma once

#include <string>
#include <vector>
#include <memory>

namespace theoria { namespace config {

/* Base configuration variable resolver.
 * 
 * Theoria configuration files can contain varaibles of the form _$variable_ ('first variables') and
 * _$$variable_ ('last variables') . Resolvers determine how a variable is bound to a value. 
 * Resolvers can be chained together in a sequence so that the value of a variable can be
 * determined by walking a list of different resolvers. 
 *
 * _First Varialbles_ ($var)  are bound by the first resolver that recognizes the variable
 * _Last Variables_ ($$var) are bound by the last resolver that recognizes the variable 
 *
 * Example ConfigVariableResolvers are: <CmdLineResolver>, <EnvVarResolver>, <ConfigVariableResolver>, <TomlResolver>  
 *
 * Say you want your project to be configurable from both the command line and encironment and you want the command line
 * to take precedence. This means if a variable is give a value on the comand line ignore any value in the env.
 *
 * For this you should use: CmdLineResolver -> EnvVarResolver and variables of the form $va1, $var2 and so on.
 * But suppose for some specific variable you don't want the user to change it if it is defined in the env. 
 * You can still use the same resolver chain but make this a _last variable_ $$specialVar
 *
 * A general guideline is to contruct your resolvger chain such that most variables are $first and only used
 * $$last syntax for special exceptions. Many projects can probably avoid $$last variables all together.
 *
 * Theoria will ouput a representation of your config file at runtime that shows how variables were resolved.
 * You can also run a project using: _theoria --resolve-only_ to see the same with out executing the code
 */
class ConfigVariableResolver 
{
public:

    using Result = std::pair<const ConfigVariableResolver*, std::string> ;
    using ResultVec = std::vector<Result> ;
   
    /*
     * Resolve variable
     * @var is the variable and it must begin with '$' character 
     *
     * @return the value of the variable.  If _var_ is not found then the _var_ itself is returned
     */
    std::string resolve(const std::string& var) const ;

    /*
     * Resolve variable across all resolvers in the chain and return vector of all successful resolutions
     * in order of precedence as dictated by whether $'var or $$'var' was used.
     *
     * @var is the variable and it must begin with '$' 
     * 
     * @return vector of resolutions such that the first entry is the value that would have been resolved
     * the second is the alternative and so on. 
     */
     ResultVec resolveAll(const std::string& var) const ;

 
    /*
     * Abstract method that implements local resolution for a specific type of resolver. 
     * @name is the name of the varaible (with no leading '$') 
     *
     * @return std::pair<"'resolver'", "'value'"> if found or std::pair<"",""> if not found
     */
    virtual Result lookup(const std::string& name) const = 0 ;

    /*
     * Return name of the resolver
     */
    virtual String name() const = 0 ;

    
    void append(ConfigVariableResolver* resolver) ;

private:

    Result resolveFirst(const std::string& name) const ;
    Result resolveLast(const std::string& name) const ;


    ConfigVariableResolver* _next ;
} ;

/*
 * Resolves variables from the os environment
 */
class EnvVarResolver : public ConfigVariableResolver
{
public:

    Result lookup(const std::string& name) const override ;
    virtual String name() const override ;

} ;

/*
 * Resolves variables set from the theroia command line
 */
class CmdLineResolver : public ConfigVariableResolver
{
public:

    Result lookup(const std::string& name) const override;
    virtual String name() const override ;

} ;

/*
 * Raises runtime error "Variables have been disallowed"
 */
class DisallowResolver : public ConfigVariableResolver
{
public:

    Result lookup(const std::string& name) const override;
    virtual String name() const override ;

} ;

/*
 * Disables resolution by simply returning the variable name
 */
class DisableResolver : public ConfigVariableResolver
{
public:

    Result lookup(const std::string& name) const override;
    virtual String name() const override ;

} ;

/* Resolves variables from a TOML file. Supports
   nested access using dot notation: E.g., $a.b
*/


class TOMLResolver : public ConfigVariableResolver
{
public:
    
    TOMLResolver(const std::string& tomlFilePath) ;
    TOMLResolver(std::istream& is) ;

    Result lookup(const std::string& name) const override;
    virtual String name() const override ;


private:

    class TOMLResolverImpl ;
    using ImplPtr = std::unique_ptr<TOMLResolverImpl>  ;
    
   ImplPtr _impl ;
};

}} //namespace theoria::config
