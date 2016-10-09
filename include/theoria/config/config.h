#pragma once

#include <theoria/except/except.h>

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <stack>


namespace theoria { namespace config {

class Config ;
class ConfigBuilder ;
class ConfigVariableResolver ;

////
//T must be something that can be read from a stream or you will get a compile error
template <typename T>
T convert(const std::string & val)
{
    T result ;
    std::istringstream iss(val) ;
    iss >> result ;
    return result ;  
}

/*
"$var - is a first variable. It's value is taken from the first resolver that contains it
"$$var - is a last variable. It's value can be overriden by later resolvers
"$var=default is a first variable which gets a default value only if no resolver contains it
$$var=default is a last variable which gets a default value only if no resolver contains it
$var1=$var2, $$var1=$var2, $var1=$$var2, $$var1=$$var2 are all legal 
$var and $$var can both appear in the same config because variable is only bound locally. In other words, every occurence is rebound.

The following resolvers are provided. You can create new ones

CmdLineResolver - resolves from values passed on cmd line in the form var1=val1, var2=val2, ...
EnvVarResolver - resolves variables from the OS env 
ConfigVariableResolver - resolves variables defined in the config file itself. 
TomlResolver - resolves variables from external TOML file. This can be used even if you don't use TOML as your primary config syntax 
*/


//Builds a chain of reslovers. Used by Config Builder
class ConfigVariableResolverBuilder 
{
} ;

//Base class of resolvers. An implementation implements lookup which is responsible
//for locally resolving a variable and return <"resolver", "value"> or <"", ""> if it 
//does not contain a value. This allows Theoria to document how variables were resolved
//as a debugging aid
 

class ConfigBuilder 
{
public:

    void pushConfig(const std::string& name, const std::string& desc="") ;
    void addAtrr() ;
    void popAsChild(); 
    void setName(const std::string& name) ;
    void setDesc(std::string& name) ;

private:

    std::stack<Config*> _stack;
    ConfigVariableResolver* _resolverChain ;
    
}; 


/*
A data structure representing runtime configuration. Config is the result of 
parsing a file in some format supprted by a config builder and resolving variable references
using some chain of ConfigVariableResolvers. 

Thus when a Config object arrives at a component's init it is fully specifified (variable free)
provided the varibales could be resolved.
*/

class Config
{
private:

    using Text = std::string ; //Placeholder for future Text class

    struct Attr
    {
        Attr(const std::string& name_, const std::string& value_, const std::string& src_ = "literal"):
            name(name_), value(value_), source(src_) {}
            
        std::string name ;
        std::string value ;
        std::string source ;
    } ;

    using Attrs = std::vector<Attr>  ;
    using Children = std::vector<Config*> ;

 public:

    Config() ;
    ~Config() ; 
    
    using ConstConfigList = std::vector<const Config*> ;
    using ConfigPredicate = bool (*) (const Config& config) ;

    Attrs::const_iterator findAttr(const std::string& name) const {
        return std::find_if(_attrs.cbegin(), _attrs.cend(), [name](auto x) { return x.name == name; }) ;
    }  

    
    const Config* getParent() noexcept 
    {
        return _parent ;
    }

    ConstConfigList getChildren() {return ConstConfigList(_children.cbegin(), _children.cend()) ;}
    ConstConfigList getChildren(ConfigPredicate predicate) ;

    ConstConfigList getSiblings() ;
    ConstConfigList getSiblings(ConfigPredicate predicate)  ;

    bool isRoot() const noexcept { return _parent == nullptr ; }
    bool isLeaf() const noexcept { return _children.empty() ; }
    
    const std::string& name() const noexcept {return _name;}
    std::string desc() const noexcept {return _desc; }
    bool hasAttr(const std::string& name) const {return findAttr(name) != _attrs.cend() ;}
    
    template <class T>
    T getAttr(const std::string& name) const 
    {
        auto iter = findAttr(name) ;
        if (iter == _attrs.cend()) 
        {
            throw RUNTIME_ERROR("Config: [%s] Attr: [%s] not found.", this->name().c_str(), name.c_str()) ;
        }

        return convert<T>(iter->name) ;
    }

    template <class T>
    T getAttr(const std::string& name, const T& def) const noexcept
    {
        auto iter = findAttr(name) ;
        return (iter != _attrs.end()) ?  convert<T>(iter->value) : def ;
    }

    std::string getAttrAsStr(const std::string& name, const std::string& def = "") const noexcept
    {
        auto iter = findAttr(name) ;
        return (iter != _attrs.end()) ? iter->value :  def ;      
    }

    int getAttrAsInt(const std::string& name, int def = 0) const 
    {
        auto iter = findAttr(name) ;
        if (iter == _attrs.end()) 
        {
            return def;
        }
        char * err = 0 ;
        int val = strtol(iter->value.c_str(), &err, 10) ;
        if (err && *err)
            throw RUNTIME_ERROR("Config: [%s] Attr: [%s] not an integer.", this->name().c_str(), name.c_str()) ;
        return val ;
    }

    double getAttrAsDbl(const std::string& name, double def = 0.0) const 
    {
        auto iter = findAttr(name) ;
        if (iter == _attrs.end()) 
        {
            return def;
        }
        char * err = 0 ;
        int val = strtod(iter->value.c_str(), &err) ;
        if (err && *err)
            throw RUNTIME_ERROR("Config: [%s] Attr: [%s] not a double.", this->name().c_str(), name.c_str()) ;
        return val ;
    }

    void toTOML(std::ostream& out) const ;

private:
    
    friend class ConfigBuilder ; 
    Config(std::string name, std::string desc, Attrs attrs) ;
 

   
    std::string _name ;
    Text _desc ; 
    Config* _parent ;
    Children _children ;
    Attrs _attrs ;
    
} ;

}} //namespace theoria::config


