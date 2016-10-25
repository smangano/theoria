#include <theoria/config/Config.h>
#include <theoria/except/except.h>

using namespace theoria;
using namespace config ;

using ConstConfigList = Config::ConstConfigList ;

Config::Config() 
    : _parent(nullptr)
{
}

Config::Config(std::string const& name, std::string const& desc)
    : _name(name), _desc(desc), _parent(nullptr)
{
}

Config::~Config() 
{
    _parent = nullptr ;
    for(auto child : _children) 
        delete child ;
}


void Config::addAttr(const std::string& name, const std::string& value, const std::string& type) 
{
    if (hasAttr(name))
        throw RUNTIME_ERROR("Could not addAttr [%s] to config [%s] : Name already exists", 
                             name.c_str(),  this->name().c_str()) ;
     _attrs.push_back(Config::Attr(name, value, type)) ;
}

void Config::addChild(Config* child) 
{
    if (hasChild(child))
        throw RUNTIME_ERROR("Could not addChild [%s] to config [%s] : Child already exists", 
                             child->name().c_str(),  this->name().c_str()) ;
   _children.push_back(child) ; 
   child->_parent = this ;
}

ConstConfigList Config::getChildren(const ConfigPredicate& predicate) const
{
    ConstConfigList result ;
    std::copy_if(_children.cbegin(), _children.cend(), std::back_inserter(result), predicate) ;
    return result ;
}

ConstConfigList Config::getSiblings() const 
{
    if (getParent() == nullptr)
        return ConstConfigList() ;
    return getParent()->getChildren([this] (const Config *c) {return c != this;}) ;       
}

ConstConfigList Config::getSiblings(const ConfigPredicate& predicate) const
{
    if (getParent() == nullptr)
        return ConstConfigList() ;
    return getParent()->getChildren([this, predicate] (const Config *c) {return c != this && predicate(c);}) ;       
}

bool Config::isArray() const
{
    return false ;
}

std::ostream& _tomlTableName(std::ostream& os, const Config* c) 
{
    if (c.getParen())
         _tomlTableName(os, c.getParen()) << "." ;
    os << c.name() ;
    return os ;
}

void Config::toTOML(std::ostream& os) const
{
    if (getParent() != nullptr)
    {
        if (getParent().isArray) os << "[" ;
        os << "[" << _tomlTableName(os, this) << "]"
        if (getParent().isArray) os << "]" ;
        os << "\n" ;
    }
    for (auto attr = cbeginAtrr(), end = cendAttr(); attr != end ; ++attr)
    {
        attr->toTOML() ;
    }
    for (child : getChildren()) {
        child->toTOML(os) ;    
    }
    
}


////
//ConfigArray

ConfigArray::ConfigArray(const std::string& name) :
    Config(name, "") {}

bool ConfigArray::isArray() const 
{
    return true ;
}

void ConfigArray::toTOML(std::ostream& os) const
{
    //pass through to elements
    for (child : getChildren()) {
        child->toTOML(os) ;    
    }
}

