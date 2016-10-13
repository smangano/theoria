#include <theoria/config/builder.h>

using namespace theoria;
using namespace config;

using ConfigPtr = ConfigBuilder::ConfigPtr ;

void ConfigBuilder::pushConfig(const std::string& name, const std::string& desc) 
{
}

void ConfigBuilder::addAtrr(const std::string& name, const std::string value, const std::string type) 
{
}

void ConfigBuilder::setAttrName(const std::string& name, const std::string& newName) 
{
}

void ConfigBuilder::setAttrValue(const std::string& name, const std::string& newValue) 
{
}

void ConfigBuilder::setAttrtype(const std::string& name, const std::string& newType) 
{
}

void ConfigBuilder::popAsChild() 
{
}

void ConfigBuilder::pop() 
{
}

void ConfigBuilder::setName(const std::string& name) 
{
}

void ConfigBuilder::setDesc(std::string& name) 
{
}

ConfigPtr ConfigBuilder::top() 
{
}

