#pragma once

#include <theoria/core/primitives.h>

#include <vector>

namespace theoria { namespace core {

/*
 * class Dependencies
 * -----------------
 * Used by components during initialization to advertise there requirements for other components. 
 * Dependencies can be loose if they only require they conform to some type or tight if they name a specific implementation
 * Dependencies can be optional if the component can continue (possibly in a reduced functional state) without the dependency being
 * satisfied. 
 * 
 * Usage Examples:
 *
 *       return Dependencies().loose("Type1").loose("Type2").default("TheFoo").strict("Type3", "ASpecificType3") 
 *
 * If you prefer a specific implementation but will settle for a generic one it is perfectly fine to do this:
 *
 *       return  Dependencies().strict("Type1", "Impl1",Dependent::optional).loose("Type1") ;
 * 
 * If you are willing to do all together without any:
 *  
 *       return  Dependencies().strict("Type1", "Impl1",Dependent::optional).loose("Type1",Dependent::optional) ;
 *        
 */
class Dependencies
{
public:

	struct Dependent
	{
		Dependent(const TypeName& type_, const SubTypeName& subtype_, int optional_ = false)
        	: type(type_), subtype(subtype_), optional(optional_) {}

        Dependent(const TypeName& type_, int optional_=false)
            : type(type_), subtype(type_), optional(optional_) {}


        bool required() const { return !optional; }

        std::string str() const ;

        explicit operator std::string () { return str(); }

		TypeName type ;
		SubTypeName subtype ;
		bool optional ;

	} ;

private:

    using DepsList = std::vector<Dependent> ;

public:

    using const_iterator = DepsList::const_iterator ;
    
	Dependencies() {} 
	Dependencies(Dependencies&& other) noexcept :
        _deps(std::move(other._deps)) {} 
	Dependencies(const Dependencies& other) noexcept :
        _deps(other._deps) {} 

    Dependencies& operator = (Dependencies&& other) = default ;
    Dependencies& operator = (Dependencies& other) = default ;

	/**
     * Add a dependent to a component of the given type. If there are multiple components of this type it will pick the first one that
     * has existing dependencies otherwise the first one it sees.
     */
	Dependencies& loose(const TypeName& type, bool optional=false) {
        _deps.push_back(Dependent(type,"",optional)) ;
        return *this;
    }

	/**
     * Add a dependent to a component of the given type  such that the type matches it's subtype . Such a component can be thought of as the default component of that
     * type and there can be only 0 or 1. 
     */
    Dependencies& def(const TypeName& name, bool optional=false) {
        _deps.push_back(Dependent(name,name, optional)) ;
        return *this;
    }

	/**
	 * Add a dependent with the specified type and subtype. There can be only 0 or 1. This is a strict dependency because the dependent is anouncing that one and only
     * one implementation will do
     */
    Dependencies& strict(const TypeName& type,const  SubTypeName& name, bool optional=false) {
        _deps.push_back(Dependent(type,name, optional)) ; 
        return *this;
    }

    const_iterator begin() const {return _deps.begin();}
    const_iterator end() const {return _deps.end();}
  
    int size() const { return _deps.size(); }

    const Dependent& operator [] (int idx) const {return _deps[idx]; }

    friend bool operator==(const Dependencies& a, const Dependencies& b) ;

private:

	DepsList _deps ;
} ;

inline bool operator==(const Dependencies::Dependent& a, const Dependencies::Dependent& b) 
{
    return a.type == b.type && a.subtype == b.subtype && a.optional == b.optional ; 
}

inline bool operator==(const Dependencies& a, const Dependencies& b) 
{
    return a._deps == b._deps ;
}

}} //namespace theoria::core

std::ostream& operator <<(std::ostream& os, const theoria::core::Dependencies::Dependent& dep) ;
