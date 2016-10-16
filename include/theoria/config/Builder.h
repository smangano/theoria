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

    using ConfigPtr = std::shared_ptr<Config> ;

    /* Create a config node with name and description and push on to the node stack
     * @name the name of the node
     * @desc the node descrition
     */
    void pushConfig(const std::string& name, const std::string& desc="") ;

    /* Add a attribute to the config node on top of the stack
     * @name attribute name
     * @value attribute value
     * @type optional type hint
     */
    void addAtrr(const std::string& name, const std::string value, const std::string type="") ;

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
    void setDesc(std::string& name) ;

    /* The node on top of the stack
     */
    ConfigPtr top() ;

    Config* releaseAll() ;

private:

    std::stack<ConfigPtr> _stack;

    ConfigVariableResolver* _resolverChain ;
    
}; 

class TOMLConfigBuilder : private ConfigBuilder
{
public:

    TOMLConfigBuilder() ;

    Config* parse(const std::string& filename) ; 
} ;

}}