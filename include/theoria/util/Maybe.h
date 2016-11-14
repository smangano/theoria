#pragma once

/*
 * An object that might be home but will throw an exception if not. 
 * 
 * "I just met you and this is crazy but here's my number so call me maybe."
 */

#include <theoria/except/except.h>

namespace theoria { 

//Forward
namespace core { class Component; } 

namespace util {

/**
 * Represents the conditional existence of a type T. A safer alternative to using nullptr.
 */
template <class T>
class Maybe
{
public:


    /**
     * Bind obj to Maybe without recording location where bound
     */
    explicit Maybe(T* obj): _obj(obj), _where("") {} 

    /**
     * Bind obj to Maybe and record location where bound if provided
     */
    explicit Maybe(T* obj, const char *where) : _obj(obj), _where(where?where:"") {}  

    /** 
     * Bind an object of type Base* which is maybe a T*. Use dynamic cast
     * to determine if it is the case. 
     */
    template <typename Base>
    explicit Maybe(Base* obj): _obj(nullptr), _where("")
    {
        _obj = dynamic_cast<T*>(obj) ;
    }

    /** 
     * Bind an object of type Base* which is maybe a T*. Use dynamic cast
     * to determine if it is the case. Record location where bound if provided. 
     */
    template <typename Base>
    explicit Maybe(Base* obj, const char * where): _obj(nullptr), _where(where?where:"")
    {
        _obj = dynamic_cast<T*>(obj) ;
    }

    /**
     * True if bound
     */
    operator bool() const
    {
       return _obj != nullptr ; 
    }

    /**
     * Where bound, if known
     */
    const char *where() {return _where;}

    /**
     * Dereference. Throws std::runtime_error if not bound
     */
    T& operator *() 
    {
        if (_obj) return *_obj ;
        throw RUNTIME_ERROR("Maybe not! Object was not bound at [%s]", where()) ;
    }

    /**
     * Const dereference. Throws std::runtime_error if not bound
     */
    const T& operator *() const
    {
        if (_obj) return *_obj ;
        throw RUNTIME_ERROR("Maybe not! Object was not bound at [%s]", where()) ;
    }

    /**
     * Pointer dereference. Throws std::runtime_error if not bound
     */
    T* operator ->() {return &(**this);}

    /**
     * Const Pointer dereference. Throws std::runtime_error if not bound
     */
    const T* operator ->() const {return &(**this);}


    /**
     * Dereference but if not bound substitute other instead of throwing exception
     */
    T& valueOr(T& other) 
    {
        if (_obj) 
            return *_obj ;
        return other ;
    }

    /**
     * Const dereference but if not bound substitute other instead of throwing exception
     */
    const T& valueOr(const T& other) const  
    {
        if (_obj) 
            return *_obj ;
        return other ;
    }

    /**
     * Dynamic cast to Derived. Throws runtime_error if not bound. Throws bad_cast if not Derived.
     */
    template <typename Derived>
    Derived& as() 
    {
        T& self =  **this ;
        return dynamic_cast<Derived&>(self) ;
    }

    /**
     * Dynamic cast to const Derived. Throws runtime_error if not bound. Throws bad_cast if not Derived.
     */
    template <typename Derived>
    const Derived& as() const 
    {
        const T& self = **this ;
        return dynamic_cast<const Derived&>(self) ;
    }

    /**
     * Cast to T2. Throws runtime_error if not bound or if cast fails.
     * NOTE: T needs to have a cast method. Components have this functionality and are the primary use case for Maybe.
     */
    template <typename T2>
    T2& cast() 
    {
        T& self =  **this ;
        return self.cast<T2>() ;
    }

    /**
     * Cast to const T2. Throws runtime_error if not bound or if cast fails.
     * NOTE: T needs to have a cast method. Components have this functionality and are the primary use case for Maybe.
     */
    template <typename T2>
    const T2& cast() const 
    {
        const T& self = **this ;
        return self.cast<T2>() ;
    }

    /**
     * Get underlying ptr
     */
    T* get() {return _obj;}

    /**
     * Get underlying const ptr
     */
    const T* get() const {return _obj;}

    /**
     * Reset underlying pointer
     */
    void reset(T* obj) { _obj = obj;} 

private:

    T* _obj ;
    const char * _where ;
} ;

}} 
