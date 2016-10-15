#pragma once

#include <theoria/except/except.h>
#include <theoria/util/conversions.h>

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>


namespace theoria { namespace config {

class Config ;
class ConfigBuilder ;
class ConfigVariableResolver ;


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
        Attr(const std::string& name_, const std::string& value_, const std::string& type_ ="", const std::string& src_ = "literal"):
            name(name_), value(value_), type(type_), source(src_) {}
            
        std::string name ;    
        std::string value ;   
        std::string type ;    //optiona indication of value's type
        std::string source ;  //"literal" or if value was resolved from a variable then "variable[resolver]"
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

        return util::convert<T>(iter->name) ;
    }

    template <class T>
    T getAttr(const std::string& name, const T& def) const noexcept
    {
        auto iter = findAttr(name) ;
        return (iter != _attrs.end()) ?  util::convert<T>(iter->value) : def ;
    }

    std::string getAttrAsStr(const std::string& name, const std::string& def = "") const noexcept
    {
        auto iter = findAttr(name) ;
        return (iter != _attrs.end()) ? iter->value :  def ;      
    }

    int getAttrAsInt(const std::string& name, int def = 0) const 
    {
        return util::convertToInt(findAttr(name), _attrs.end(), "getAttr", name, def) ;
    }

    double getAttrAsDbl(const std::string& name, double def = 0.0) const 
    {
        return util::convertToDbl(findAttr(name), _attrs.end(), "getAttr", name, def) ;
    }

    void toTOML(std::ostream& out) const ;

private:
    
    friend class ConfigBuilder ; 
    Config(const std::string& name, const std::string& desc) ;

    void addAttr(const std::string& name, const std::string& value, const std::string& type="") ;
    void addChild(Config* child) ;

    Attrs::iterator findAttr(const std::string& name) {
        return std::find_if(_attrs.begin(), _attrs.end(), [name](auto x) { return x.name == name; }) ;
    }  
 
    Attrs::iterator endAttr() {
        return _attrs.end() ;
    }

    void setName(const std::string& name) {_name = name;}
    void setDesc(const std::string& desc) {_desc = desc;}

    std::string _name ;
    Text _desc ; 
    Config* _parent ;
    Children _children ;
    Attrs _attrs ;
    
} ;

}} //namespace theoria::config


