#pragma once

/*
 * An object that might be home but will throw an exception if not. 
 * 
 * "I just met you and this is crazy but here's my number so call me maybe."
 */

#include <theoria/except/except.h>

namespace theoria { namespace util {

template <class T>
class Maybe
{
public:

    explicit Maybe(T* obj): _obj(obj), _where("") {} 

    Maybe(T* obj, const char *where) : _obj(obj), _where(where?where:"") {}  

    template <typename Base>
    explicit Maybe(Base* obj): _obj(nullptr), _where("")
    {
        _obj = dynamic_cast<T*>(obj) ;
    }

    template <typename Base>
    explicit Maybe(Base* obj, const char * where): _obj(nullptr), _where(where?where:"")
    {
        _obj = dynamic_cast<T*>(obj) ;
    }

    operator bool() const
    {
       return _obj != nullptr ; 
    }

    const char *where() {return _where;}

    T& operator *() 
    {
        if (_obj) return *_obj ;
        throw RUNTIME_ERROR("Maybe not! Object was not bound at [%s]", where()) ;
    }

    const T& operator *() const
    {
        if (_obj) return *_obj ;
        throw RUNTIME_ERROR("Maybe not! Object was not bound at [%s]", where()) ;
    }

    T* operator ->() {return &(**this);}
    const T* operator ->() const {return &(**this);}


    T& valueOr(T& other) 
    {
        if (_obj) 
            return *_obj ;
        return other ;
    }

    const T& valueOr(const T& other) const  
    {
        if (_obj) 
            return *_obj ;
        return other ;
    }

    template <typename Derived>
    Derived& as() 
    {
        T& self =  **this ;
        return dynamic_cast<Derived&>(self) ;
    }

    template <typename Derived>
    const Derived& as() const 
    {
        const T& self = **this ;
        return dynamic_cast<const Derived&>(self) ;
    }

    T* get() {return _obj;}
    const T* get() const {return _obj;}

    void reset(T* obj) { _obj = obj;} 

private:

    T* _obj ;
    const char * _where ;
} ;

}} 
