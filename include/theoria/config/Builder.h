#pragma once

#include <memory>
#include <stack>
#include <vector>
#include <string>

namespace theoria { namespace config {

class Config ;
class ConfigVariableResolver ;

/**
 * Provides support for building config files from hierarchical data files like TOML, XML, YAML, etc.
 */
class ConfigBuilder 
{
public:

    /**
     * Type of pointer to Config
     */
    using ConfigPtr = std::unique_ptr<Config> ;
    
    /**
     * Construct a builder with no initial resolver
     */
    ConfigBuilder():
        _stack(std::move(this->_vec)), _resolverChain(nullptr) {}

    /**
     * Construct a builder with resolver as head
     * @param pResolver pointer to resolver
     */
    ConfigBuilder(ConfigVariableResolver* pResolver):
        _stack(std::move(this->_vec)), _resolverChain(pResolver) {}

    /**
     * Set the head resolver. Overwrites previous resolver
     * @param pResolver the new resolver
     */
    void setResolver(ConfigVariableResolver* pResolver) {_resolverChain = pResolver;}     

    /** 
     * Create a config node with name and description and push on to the node stack
     * @param name the name of the node
     * @param desc the node description
     */
    void pushConfig(const std::string& name, const std::string& desc="") ;

    /**
     * Push an existing node to augment
     * @param config a Config node
     */
    void pushConfig(Config* config) ;

    /**
     * Create a ConfigArray node with name and push on to the node stack
     * @param name the name of the array
     */
    void pushConfigArray(const std::string& name) ;

    /**
     * Add a attribute to the config node on top of the stack
     * @param name attribute name
     * @param value attribute value
     * @param type optional type hint
     */
    void addAttr(const std::string& name, const std::string value, const std::string type="") ;

    /**
     * Change attributes name
     * @param name current name
     * @param newName the new attribute name
     */
    void setAttrName(const std::string& name, const std::string& newName) ;

    /**
     * Change attributes value
     * @param name the name of the attribute to change
     * @param newValue the value to change to
     */
    void setAttrValue(const std::string& name, const std::string& newValue) ;

    /** 
     * Change attributes type
     * @param name the name of the attribute to change
     * @param newType the type name to change to (string, int, bool, double)
     */
    void setAttrType(const std::string& name, const std::string& newType) ;

    /** 
     * Change attribute's source to variable within resolver
     */
    void setAttrSource(const std::string& name, const std::string& variableName, const std::string& resolverName) ;

    /**
     * Pop a node off the stack and attach it as a child to the new top node
     * @param allowDups Normally Config nodes must be unique by name. Set true to override.  
     *                  NOTE: if parent is an array dups will automatically to be allowed even if allowDups==false
     */
    void popAsChild(bool allowDups=false); 

    /**
     * Pop the top node. It will be lost if you need not save somewhere by first calling top
     */
    void pop() ;

    /**
     * Change the name of the node on the top of the stack
     * @param name the new name for node
     */
    void setName(const std::string& name) ;

    /**
     * Change the description of the node on the top of the stack
     * @param desc the new description for the node
     */
    void setDesc(const std::string& desc) ;

    /**
     * The node on top of the stack
     * @return top node on the config stack 
     * @throw std::runtime_error if stack is empty
     */
    ConfigPtr& top() ;

    /**
     * Call popChild repeatedly until stack is empty then release the top element from its owning unique ptr and return it.
     * @return the root node (the node deepest in the stack when this function was called) or nullptr if stack empty
     */
    Config* releaseAll() ;

    /**
     * Return the head resolver of this builder's resolver chain. 
     * Normally variable resolution is taken care of automatically so this method not typically required. 
     * Useful for unit testing.
     * @return the head resolver
     */
    const ConfigVariableResolver* resolver() const {return _resolverChain;}

private:

    std::string resolve(const std::string& valueOrVar) ;
    std::vector<ConfigPtr> _vec ;
    std::stack<ConfigPtr, std::vector<ConfigPtr>> _stack;

    ConfigVariableResolver* _resolverChain ;
    
}; 

}}
