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

void Config::addChild(Config* child, bool allowDups) 
{
    if (getChild(child->name())) 
        if (!allowDups || hasChild(child)) //allowDups is only okay if not the same object
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

const Config* Config::getChild(const std::string& name) const
{
    auto iter = std::find_if(_children.cbegin(), _children.cend(), [name] (const Config* c) {return c->name() == name;}) ;
    if (iter != _children.cend())
        return *iter;
    return nullptr ;        
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

struct toTomlName
{
    toTomlName(const Config& c_) : c(c_) {}
    const Config& c;
} ;

std::ostream& operator <<(std::ostream& os, const toTomlName& n) 
{
    if (n.c.getParent() && n.c.getParent()->getParent())
        os << toTomlName(*n.c.getParent()) << "." ;
    os << n.c.name() ;
    return os ;
}

void Config::toTOML(std::ostream& os) const
{
    if (getParent() != nullptr)
    {
        if (getParent()->isArray()) os << "[" ;
        os << "[" << toTomlName(*this) << "]" ;
        if (getParent()->isArray()) os << "]" ;
        os << "\n" ;
    }
    for (auto attr = beginAttr(), end = endAttr(); attr != end ; ++attr)
    {
        attr->toTOML(os) ;
    }
    for (auto child : getChildren()) {
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
    for (auto child : getChildren()) {
        child->toTOML(os) ;    
    }
}

