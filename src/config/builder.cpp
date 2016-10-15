#include <theoria/config/builder.h>

using namespace theoria;
using namespace config;

using ConfigPtr = ConfigBuilder::ConfigPtr ;

void ConfigBuilder::pushConfig(const std::string& name, const std::string& desc) 
{
    _stack.push(std::make_shared_ptr(Config(name, desc))) ;
}

void ConfigBuilder::addAtrr(const std::string& name, const std::string value, const std::string type) 
{
    _stack.top()->addAttr(name, value, type) ;
}

void ConfigBuilder::setAttrName(const std::string& name, const std::string& newName) 
{
    auto iter =  _stack.top()->findAttr(name) ;
    if (iter !=  _stack.top()->endAttr()) {
        iter->name = newName ;
    }
    throw RUNTIME_ERROR("Could not setAttrName [%s] to [%s] in config [%s] as no attr found with name [%s]", 
                         name.c_str(), newName.c_str(), _stack.top()->name(), name.c_str()) ;
}

void ConfigBuilder::setAttrValue(const std::string& name, const std::string& newValue) 
{
    auto iter =  _stack.top()->findAttr(name) ;
    if (iter !=  _stack.top()->endAttr()) {
        iter->value = newValue ;
    }
    throw RUNTIME_ERROR("Could not setAttrValue [%s] value to [%s] in config [%s] as no attr found with name [%s]", 
                         name.c_str(), newValue.c_str(), _stack.top()->name(), name.c_str()) ;
}

void ConfigBuilder::setAttrType(const std::string& name, const std::string& newType) 
{
    auto iter =  _stack.top()->findAttr(name) ;
    if (iter !=  _stack.top()->endAttr()) {
        iter->type = newType ;
    }
    throw RUNTIME_ERROR("Could not setAttrType [%s] type to [%s] in config [%s] as no attr found with name [%s]", 
                         name.c_str(), newType.c_str(), _stack.top()->name(), name.c_str()) ;
}

void ConfigBuilder::setAttrSource(const std::string& name, const std::string& variableName, const std::string& resolverName) 
{
    auto iter =  _stack.top()->findAttr(name) ;
    if (iter !=  _stack.top()->endAttr()) {
        iter->source = variableName + " [" + resolverName + "]" ;
    }

    throw RUNTIME_ERROR("Could not setAttrSource [%s] source to [%s] in config [%s] as no attr found with name [%s]", 
                         name.c_str(),variableName + " [" + resolverName + "]", _stack.top()->name(), name.c_str()) ;
}


void ConfigBuilder::popAsChild() 
{
    ConfigPtr child = _stack.top() ;
    _stack.pop() ;
    _stack.top()->addChild(child.release()) ;
}

void ConfigBuilder::pop() 
{
    _stack.pop() ;
}

void ConfigBuilder::setName(const std::string& name) 
{
    _stack.top()->setName(name) ;
}

void ConfigBuilder::setDesc(std::string& name) 
{
   _stack.top()->setDesc(name) ;
}

ConfigPtr ConfigBuilder::top() 
{
    return  _stack.top() ;
}

