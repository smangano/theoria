#pragma once

#include <memory>
#include <stack>
#include <string>

namespace theoria { namespace config {

class Config ;
class ConfigVariableResolver ;

/* Provides support for building config files from hierachical data files like TOML, XML, YAML, etc.
 */
class ConfigBuilder 
{
public:

    using ConfigPtr = std::unique_ptr<Config> ;

    ConfigBuilder():
        _resolverChain(nullptr) {}

    ConfigBuilder(ConfigVariableResolver* pResolver):
        _resolverChain(pResolver) {}

    

    /* Create a config node with name and description and push on to the node stack
     * @name the name of the node
     * @desc the node descrition
     */
    void pushConfig(const std::string& name, const std::string& desc="") ;

    /* Create a ConfigArray node with name and push on to the node stack
     * @name the name of the array
     */
    void pushConfigArray(const std::string& name) ;

    /* Add a attribute to the config node on top of the stack
     * @name attribute name
     * @value attribute value
     * @type optional type hint
     */
    void addAttr(const std::string& name, const std::string value, const std::string type="") ;

    /* Change attributes name
     */
    void setAttrName(const std::string& name, const std::string& newName) ;

    /* Change attributes value
     */
    void setAttrValue(const std::string& name, const std::string& newValue) ;

    /* Change attributes type 
     */
    void setAttrType(const std::string& name, const std::string& newType) ;

    /* Change attribute's source to variable within resolver
    */
    void setAttrSource(const std::string& name, const std::string& variableName, const std::string& resolverName) ;

    /* Pop a node off the stack and attach it as a chile to the new top node
     */
    void popAsChild(); 

    /* Pop the top node. It will be lost if you need not save somewhere by first calling top
     */
    void pop() ;

    /* Change the name of the node on the top of the stack
     */
    void setName(const std::string& name) ;

    /* Change the description of the node on the top of the stack
     */
    void setDesc(const std::string& name) ;

    /* The node on top of the stack
     */
    ConfigPtr& top() ;

    Config* releaseAll() ;

private:

    std::string resolve(const std::string& valueOrVar) ;

    std::stack<ConfigPtr> _stack;

    ConfigVariableResolver* _resolverChain ;
    
}; 

}}
