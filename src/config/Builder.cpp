#include <theoria/config/Builder.h>
#include <theoria/config/Config.h>
#include <theoria/config/Resolve.h>
#include <memory>
#include <string>

using namespace theoria;
using namespace config;

using ConfigPtr = ConfigBuilder::ConfigPtr ;

void ConfigBuilder::pushConfig(const std::string& name, const std::string& desc) 
{
    _stack.push(ConfigPtr(new Config(name, desc))) ;
}

void ConfigBuilder::pushConfig(Config* config) 
{
    _stack.push(ConfigPtr(config)) ;
}

void ConfigBuilder::pushConfigArray(const std::string& name) 
{
    _stack.push(ConfigPtr(new ConfigArray(name))) ;
}

void ConfigBuilder::addAttr(const std::string& name, const std::string value, const std::string type) 
{
    _stack.top()->addAttr(name, resolve(value), type) ;
}

void ConfigBuilder::setAttrName(const std::string& name, const std::string& newName) 
{
    auto iter =  _stack.top()->findAttr(name) ;
    if (iter !=  _stack.top()->endAttr()) {
        iter->name = newName ;
    }
    throw RUNTIME_ERROR("Could not setAttrName [%s] to [%s] in config [%s] as no attr found with name [%s]", 
                         name.c_str(), newName.c_str(), _stack.top()->name().c_str(), name.c_str()) ;
}

void ConfigBuilder::setAttrValue(const std::string& name, const std::string& newValue) 
{
    auto iter =  _stack.top()->findAttr(name) ;
    if (iter !=  _stack.top()->endAttr()) {
        iter->value = resolve(newValue) ;
    }
    throw RUNTIME_ERROR("Could not setAttrValue [%s] value to [%s] in config [%s] as no attr found with name [%s]", 
                         name.c_str(), newValue.c_str(), _stack.top()->name().c_str(), name.c_str()) ;
}

void ConfigBuilder::setAttrType(const std::string& name, const std::string& newType) 
{
    auto iter =  _stack.top()->findAttr(name) ;
    if (iter !=  _stack.top()->endAttr()) {
        iter->type = newType ;
    }
    throw RUNTIME_ERROR("Could not setAttrType [%s] type to [%s] in config [%s] as no attr found with name [%s]", 
                         name.c_str(), newType.c_str(), _stack.top()->name().c_str(), name.c_str()) ;
}

void ConfigBuilder::setAttrSource(const std::string& name, const std::string& variableName, const std::string& resolverName) 
{
    auto iter =  _stack.top()->findAttr(name) ;
    std::string source = variableName + " [" + resolverName + "]" ;
    if (iter !=  _stack.top()->endAttr()) {
        iter->source = source ;
    }
    
    throw RUNTIME_ERROR("Could not setAttrSource [%s] source to [%s] in config [%s] as no attr found with name [%s]", 
                         name.c_str(),source.c_str(), _stack.top()->name().c_str(), name.c_str()) ;
}


void ConfigBuilder::popAsChild(bool allowDups) 
{
    ConfigPtr child;
    child.swap(_stack.top()) ;
    _stack.pop() ;
    Config *p = nullptr ;
    try {
        p = child.release() ;
        _stack.top()->addChild(p, allowDups || _stack.top()->isArray()) ;
    } catch (...) {
        child.reset(p) ; //retake ownership!
        throw ;
    }
}

void ConfigBuilder::pop() 
{
    _stack.pop() ;
}

void ConfigBuilder::setName(const std::string& name) 
{
    _stack.top()->setName(name) ;
}

void ConfigBuilder::setDesc(const std::string& name) 
{
   _stack.top()->setDesc(name) ;
}

ConfigPtr& ConfigBuilder::top() 
{
    return  _stack.top() ;
}

Config* ConfigBuilder::releaseAll() 
{
    if (_stack.empty())
        return nullptr ;

    while (_stack.size() > 1)
        popAsChild() ;
    ConfigPtr top;
    top.swap(_stack.top()) ;
    _stack.pop() ;
    return top.release() ;
}

std::string ConfigBuilder::resolve(const std::string& valueOrVar)
{
    if (valueOrVar.size() > 0 && valueOrVar[0] == '$')
        if (_resolverChain)
            return _resolverChain->resolve(valueOrVar) ; 
    return valueOrVar ;
}

