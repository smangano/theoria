#include <theoria/config/config.h>

using namespace theoria;
using namespace config ;

Config::Config() 
{
}

Config::Config(std::string const& name, std::string const& desc)
    : _name(name), _desc(desc)
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
    _attrs.push_back(Config::Attr(name, value, type)) ;
}

void Config::addChild(Config* child) 
{
   _children.push_back(child) ; 
}
