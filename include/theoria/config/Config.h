#pragma once

#include <theoria/except/except.h>
#include <theoria/util/conversions.h>

#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <vector>


namespace theoria { namespace config {

class Config ;
class ConfigBuilder ;
class ConfigVariableResolver ;


/**
 * class Config
 *
 * A hierarchical data structure representing runtime configuration. Config is the result of 
 *  parsing a file in some format supported by a config builder and resolving variable references
 * using some chain of ConfigVariableResolvers.
 *
 * Thus when a Config object arrives at a component's init it is fully specified (variable free)
 * provided the variables could be resolved.
 */
class Config
{
protected:

    using Text = std::string ; //Placeholder for future Text class

    /**
     * Internal representation of a attribute 
     */
    struct Attr
    {
        Attr(const std::string& name_, const std::string& value_, const std::string& type_ ="", const std::string& src_ = "literal"):
            name(name_), value(value_), type(type_), source(src_) {}

        std::string decorated_val() const 
        {
            if (type == "string")
                return "\"" + value + "\"" ;
            return value ;
        }

        void toTOML(std::ostream& os) const {
            os << name  << " = " << decorated_val() ;
            if (source != "")
                os << " #" << source ;
            os << "\n" ;
        }

        std::string name ;    
        std::string value ;   
        std::string type ;    //optional indication of value's type
        std::string source ;  //"literal" or if value was resolved from a variable then "variable[resolver]"
    } ;

    using Attrs = std::vector<Attr>  ;
    using Children = std::vector<Config*> ;

 public:

    /**
     * Construct an empty config node (no attrs, no children)
     */
    Config() ;

    /**
     * Destructor
     */
    virtual ~Config() ; 
    
    /**
     * The type used for a list of Config nodes
     */
    using ConstConfigList = std::vector<const Config*> ;

    /**
     * The type used for a predicate on a Config node
     */
    using ConfigPredicate = std::function<bool (const Config *)> ;

    /**
     * Find an attribute by name
     * @return a const iterator to the attribute or endAttr() if not found
     */
    Attrs::const_iterator findAttr(const std::string& name) const {
        return std::find_if(_attrs.cbegin(), _attrs.cend(), [name](auto x) { return x.name == name; }) ;
    }  

    /**
     * The parent of this config node or nullptr if this is the top-level node
     * @return parent 
     */
    const Config* getParent() const noexcept 
    {
        return _parent ;
    }

    /**
     * Test if node has children
     * @return true if node has children
     */
    bool hasChild(const Config* child) const {return find(_children.cbegin(), _children.cend(), child) != _children.cend();}

    /**
     * Return the number of children of this node
     * @return integer number of children
     */
    int numChildren() const {return _children.size();}

    /**
     * Return all children in a list
     */
    ConstConfigList getChildren() const {return ConstConfigList(_children.cbegin(), _children.cend()) ;}

    /**
     * Return only those children that satisfy the predicate
     * @param predicate bool (const Config*) 
     * @return The list
     */
    ConstConfigList getChildren(const ConfigPredicate& predicate) const ;

    /**
     * Return child with given name.
     * @return child ptr or nullptrt if no such child
     */
    const Config* getChild(const std::string& name) const ;

    /**
     * Return siblings of this node (nodes at same level other than this)
     * @return list of sinlings (possibly empty)
     */ 
    ConstConfigList getSiblings() const ;

    /**
     * Return siblings of this node (nodes at same level other than this) that satisfy predicate
     * @return list of sinlings (possibly empty)
     */ 
    ConstConfigList getSiblings(const ConfigPredicate& predicate) const  ;

    /**
     * Test if this node is root
     * @return true if root (no parent)
     */
    bool isRoot() const noexcept { return _parent == nullptr ; }

    /**
     * Test if this node is a leaf (no children)
     * @return true if leaf
     */
    bool isLeaf() const noexcept { return _children.empty() ; }
    
    /**
     * The name of the node
     */
    const std::string& name() const noexcept {return _name;}

    /**
     * The nodes desc.
     */
    std::string desc() const noexcept {return _desc; }

    /**
     * Test if node has a attribute with given name
     */
    bool hasAttr(const std::string& name) const {return findAttr(name) != _attrs.cend() ;}

    /**
     * The number of attributes of this node
     */
    int numAttr() const {return _attrs.size();}

    /**
     * Get the attribute of specified name as the givent type T
     * @return the attribute value as type T
     * @throw std::runtime_error if no such attribute
     * @throw std::runtime_error if no legal conversion possible
     */
    template <class T>
    T getAttr(const std::string& name) const 
    {
        auto iter = findAttr(name) ;
        if (iter == _attrs.cend()) 
        {
            throw RUNTIME_ERROR("Config: [%s] Attr: [%s] not found.", this->name().c_str(), name.c_str()) ;
        }

        return util::convert<T>(iter->value) ;
    }

    /**
     *  Get the attribute of specified name as the given type T or def if no such attribute
     * @param name  the name of the attribue
     * @param def default value if no such attribute 
     * @throw std::rumtimer error if attribute exusts but can't be convereted to type T
     */
    template <class T>
    T getAttr(const std::string& name, const T& def) const noexcept
    {
        auto iter = findAttr(name) ;
        return (iter != _attrs.end()) ?  util::convert<T>(iter->value) : def ;
    }

    /**
     * Get the value of the attribute of specified name as a string or default if no such attribute exists
     * @param name the name of the attribute
     * @param def default value if name does not exist
     * This function will always return some value - never throws
     */
    std::string getAttrAsStr(const std::string& name, const std::string& def = "") const noexcept
    {
        auto iter = findAttr(name) ;
        return (iter != _attrs.end()) ? iter->value :  def ;      
    }

    /**
     * Get the value of the attribute of specified name as an int or default if no such attribute exists
     * @param name the name of the attribute
     * @param def the default value if name does not exist
     * @throw std::runtime_error if conversion to int not possible
     */
    int getAttrAsInt(const std::string& name, int def = 0) const 
    {
        return util::convertToInt(findAttr(name), _attrs.end(), "getAttr", name, def) ;
    }

    /**
     * Get the value of the attribute of specified name as a double or default if no such attribute exists
     * @param name the name of the attribute
     * @param def the default value if name does not exist
     * @throw std::runtime_error if conversion to double not possible
     */
    double getAttrAsDbl(const std::string& name, double def = 0.0) const 
    {
        return util::convertToDbl(findAttr(name), _attrs.end(), "getAttr", name, def) ;
    }

    /**
     * Get the value of the attribute of specified name as a boolean or default if no such attribute exists
     * @param name the name of the attribute
     * @param def the default value if name does not exist
     * @throw std::runtime_error if conversion to bool not possible
     */
    bool getAttrAsBool(const std::string& name, bool def = false) const 
    {
        return util::convertToBool(findAttr(name), _attrs.end(), "getAttr", name, def) ;
    }

    /**
     * Test to descriminate between a normal Config node and a ConfigArray (arry of nodes)
     * @return false as Config base is normal node 
     * @See ConfigArray
     */
    virtual bool isArray() const ;

    /**
     * Write the TOML (http://github.com/toml-lang/toml) to provided stream
     * NOTE: The source format need not have been TOML to use this conversion
     */
    virtual void toTOML(std::ostream& out) const ;

    /**
     * Return iterator over all attributes 
     * @see endAttr
     */
    Attrs::const_iterator beginAttr() const {
        return _attrs.begin() ;
    }

    /**
     * Return bounding iterator past last attribute
     * @see beginAttr
     */
    Attrs::const_iterator endAttr() const {
        return _attrs.end() ;
    }

protected:
    
    /**
     * A Config is contructed by a ConfigBuilder therefor only ConfigBuilder has
     * ability to mutate configs. 
     */
    friend class ConfigBuilder ; 
    Config(const std::string& name, const std::string& desc) ;

    
    void addAttr(const std::string& name, const std::string& value, const std::string& type="") ;
    void addChild(Config* child, bool allowDups=false) ;

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

/** 
 * An array of configs. Used where order is significan as normally child nodes have no inherent order.
 * In practice, ConfigArray is implemented in terms of Config's children with the assumption that the parser
 * that processed the config will add the children in the proper order when ordering matters.
 *
 * For example, a TOML parser is not required to present tables in any particular order but must present arrays of tables in the order
 * they appear in the document. This is where the parse must use ConfigArray rather than Config.
 */
class ConfigArray : public Config
{
public:
    using const_iterator = Children::const_iterator ;

    virtual void toTOML(std::ostream& out) const ;
    /**
     * Returns true to indicate this is an array
     */
    virtual bool isArray() const ;

    /**
     * Test if a node is in this array
     * @param elem the node to search for
     */
    bool hasElement(const Config* elem) const {return find(_children.cbegin(), _children.cend(), child) != _children.cend();}

    /**
     * Return the size of the array
     * @return number of nodes
     */
    int numElements() const {return _children.size();}

    /**
     * Return all nodes as a list
     */
    ConstConfigList getElements() const {return ConstConfigList(_children.cbegin(), _children.cend()) ;}

    /**
     * Return all nodes that satisfy predicate
     * @param predicate - a function returning true for nodes to include
     */
    ConstConfigList getElements(const ConfigPredicate& predicate) const {return getChildren(predicate);}

    /**
     * Return node at a specific index
     * @param index 0 based index
     * @return the node at the specified index
     * @throw std::out_of_range if index is < 0 or >= size 
     */
    const Config* at(int index) const { return _children.at(index) ; }
    
    /**
     * Return node at a specific index
     * @param index 0-based index. Undefined behavior if node is out of range.
     * @return the node at specified index
     */
    const Config* operator[](int index) const { return _children[index] ; }
    
    /**
     * Iterator to first node
     */
    const_iterator begin() const {return _children.cbegin(); }

    /**
     * Iterator past last node
     */
    const_iterator end() const {return _children.cend(); }

protected:
    friend class ConfigBuilder ; 

    ConfigArray(const std::string& name) ;
} ;

}} //namespace theoria::config


