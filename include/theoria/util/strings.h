#pragma once

#include <string>
#include <sstream>

namespace theoria { namespace util {


inline std::string str() {return std::string(); }

template <class T1>
inline
std::string str(const T1& v1) 
{
    std::ostringstream ss ;
    ss << v1;
    return ss.str() ; 
}

template <class T1, class T2>
inline
std::string str(const T1& v1, const T2& v2) 
{
    std::ostringstream ss ;
    ss << v1 << v2 ;
    return ss.str() ; 
}

template <class T1, class T2, class T3>
inline
std::string str(const T1& v1, const T2& v2, const T3 v3) 
{
    std::ostringstream ss ;
    ss << v1 << v2 << v3;
    return ss.str() ; 
}

template <class T1, class T2, class T3, class T4>
inline
std::string str(const T1& v1, const T2& v2, const T3& v3, const T4& v4) 
{
    std::ostringstream ss ;
    ss << v1 << v2 << v3 << v4;
    return ss.str() ; 
}

template <class T1, class T2, class T3, class T4, class T5>
inline
std::string str(const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5) 
{
    std::ostringstream ss ;
    ss << v1 << v2 << v3 << v4 << v5;
    return ss.str() ; 
}

template <class T1, class T2>
inline
std::string delim(const std::string& d, const T1& v1, const T2& v2) 
{
    std::ostringstream ss ;
    ss << v1 << d << v2 ;
    return ss.str() ; 
}

template <class T1, class T2, class T3>
inline
std::string delim(const std::string& d, const T1& v1, const T2& v2, const T3 v3) 
{
    std::ostringstream ss ;
    ss << v1 << d << v2 << d << v3;
    return ss.str() ; 
}

template <class T1, class T2, class T3, class T4>
inline
std::string delim(const std::string& d, const T1& v1, const T2& v2, const T3& v3, const T4& v4) 
{
    std::ostringstream ss ;
    ss << v1 << d << v2 << d << v3 << d << v4;
    return ss.str() ; 
}

template <class T1, class T2, class T3, class T4, class T5>
inline
std::string delim(const std::string& d,const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5) 
{
    std::ostringstream ss ;
    ss << v1 << d << v2 << d << v3 << d << v4 << d << v5;
    return ss.str() ; 
}

}} //namespace theoria::util
