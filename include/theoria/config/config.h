#pragma once

namespace theoria { namespace config {
 
class Config
{
public:

    using ConfigList = std::vector<Config*> ;
    using ConfigPredicate = bool (*) (const Config& config) ;

    Config* getParent() noexcept ;

    ConfigList getChildren() ;
    ConfigList getChildren(ConfigPredicate predicate) ;

    ConfigList getSiblings() ;
    ConfigList getSiblings(ConfigPredicate predicate)  ;

    bool isRoot() const noexcept ;
    bool isLeaf() const noexcept ;
    
    std::string name() const noexcept ;
    std::string desc() const noexcept ;

    template <class T>
    T getAttr(const std::string& name) const ;

    template <class T>
    T getAttr(const std::string& name, const T& def) const noexcept;

    std::string getA
} ;

}} //namespace theoria::config


