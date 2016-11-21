#pragma once

#include <string>
#include <vector>
#include <memory>

namespace theoria { namespace config {

/** 
 * Base configuration variable resolver.
 * 
 * Theoria configuration files can contain variables of the form _$variable_ ('first variables') and
 * _$$variable_ ('last variables ') . Resolvers determine how a variable is bound to a value. 
 * Resolvers can be chained together in a sequence so that the value of a variable can be
 * determined by walking a list of different resolvers. 
 *
 * _First Variables_ ($var)  are bound by the first resolver that recognizes the variable
 * _Last Variables_ ($$var) are bound by the last resolver that recognizes the variable 
 *
 * Example ConfigVariableResolvers are: \verbatim <CmdLineResolver>, <EnvVarResolver>, <ConfigVariableResolver>, <TomlResolver> \endverbatim  
 *
 * Say you want your project to be configurable from both the command line and environment and you want the command line
 * to take precedence. This means if a variable is give a value on the command line ignore any value in the env.
 *
 * For this you should use: CmdLineResolver -> EnvVarResolver and variables of the form $va1, $var2 and so on.
 * But suppose for some specific variable you don't want the user to change it if it is defined in the env. 
 * You can still use the same resolver chain but make this a _last variable_ $$specialVar
 *
 * A general guideline is to construct your resolver chain such that most variables are $first and only used
 * $$last syntax for special exceptions. Many projects can probably avoid $$last variables all together.
 *
 * Theoria will output a representation of your config file at runtime that shows how variables were resolved.
 * You can also run a project using: _theoria --resolve-only_ to see the same with out executing the code
 */
class ConfigVariableResolver 
{
public:

    /**
     * The result type is a pair consisting of the resolver that determined the value and the value itself
     */
    using Result = std::pair<const ConfigVariableResolver*, std::string> ;

    /**
     * A vector of resolver results
     */
    using ResultVec = std::vector<Result> ;

    /**
     * Default constructor sets next resolver to nullptr
     */
    ConfigVariableResolver() :
        _next(nullptr) {}

    /**
     * Destructor
     */
    virtual ~ConfigVariableResolver() ; 

    /**
     * Resolve variable
     * @param var is the variable and it must begin with '$' character 
     *
     * @return the value of the variable.  If _var_ is not found then the _var_ itself is returned
     */
    std::string resolve(const std::string& var) const ;

    /**
     * Resolve variable across all resolvers in the chain and return vector of all successful resolutions
     * in order of precedence as dictated by whether $'var or $$'var' was used.
     *
     * @param var is the variable and it must begin with '$' 
     * 
     * @return vector of resolutions such that the first entry is the value that would have been resolved
     * the second is the alternative and so on. 
     */
     ResultVec resolveAll(const std::string& var) const ;

 
    /**
     * Abstract method that implements local resolution for a specific type of resolver. 
     * @name is the name of the variable (with no leading '$') 
     *
     * @return std::pair<"'resolver'", "'value'"> if found or std::pair<"",""> if not found
     */
    virtual Result lookup(const std::string& name) const = 0 ;

    /**
     * Return name of the resolver
     */
    virtual std::string name() const = 0 ;

    /**
     * Append resolver to this one to form a resolver chain
     */
    void append(ConfigVariableResolver* resolver) ;

    /**
     * Return next resolver in the chain
     * @return resolver or nullptr
     */
    const ConfigVariableResolver* next() const { return _next; }

private:

    Result resolveFirst(const std::string& name) const ;
    Result resolveLast(const std::string& name) const ;


    ConfigVariableResolver* _next ;
} ;

/**
 * Resolves variables from the os environment
 */
class EnvVarResolver : public ConfigVariableResolver
{
public:


    /**
     * Resolve name by looking at env variable with same name
     */
    Result lookup(const std::string& name) const override ;

    /**
     * Returns "EnvVarResolver"
     */
    virtual std::string name() const override ;

} ;

/**
 * Resolves variables set from the theoria command line
 */
class CmdLineResolver : public ConfigVariableResolver
{
public:

    /**
     * Look up name in command line variables
     */
    Result lookup(const std::string& name) const override;

    /**
     * Return "CmdLineResolver"
     */
    virtual std::string name() const override ;

} ;

/**
 * Raises runtime error "Variables have been disallowed"
 */
class DisallowResolver : public ConfigVariableResolver
{
public:

    /**
     * Always throws
     *@throw std::runtime_error
     */
    Result lookup(const std::string& name) const override;

    /**
     * Return "DisallowResolver"
     */
    virtual std::string name() const override ;

} ;

/**
 * Disables resolution by simply returning the variable name
 */
class DisableResolver : public ConfigVariableResolver
{
public:

    /**
     * Return name un resolved
     */
    Result lookup(const std::string& name) const override;

    /**
     * Return "DisableResolver"
     */
    virtual std::string name() const override ;

} ;

/**
 * Resolves variables from a TOML file. Supports
 * nested access using dot notation: E.g., $a.b
 * You can use this resolver even if your App's
 * config is not TOML
 */
class TOMLResolver : public ConfigVariableResolver
{
public:
   
    TOMLResolver() = delete ;

    /**
     * Use specified TOLM file as source for resolver
     */
    explicit TOMLResolver(const std::string& tomlFilePath) ;

    /**
     * Use TOLM  supplied by a stream as source for resolver. Typically a std::istringstream
     */
    explicit TOMLResolver(std::istream& is) ;

    ~TOMLResolver() ;

    /**
     * Implement look on the TOML representation
     */
    Result lookup(const std::string& name) const override;

    /**
     * Return "TOMLResolver"
     */ 
    virtual std::string name() const override ;


private:


    class TOMLResolverImpl ;
    using ImplPtr = std::unique_ptr<TOMLResolverImpl>  ;
    
   ImplPtr _impl ;
};

}} //namespace theoria::config
