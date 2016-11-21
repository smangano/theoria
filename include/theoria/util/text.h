/*
 * Theoria - A C++ Application Container for the Real and Virtual World
 *
 * Copyright (c) 2016 theoriacpp.com
 * 
 * This library is released under the Apache License v. 2.0. See LICENSE in top level directory of this project.
 * or https://github.com/smangano/theoria/blob/master/LICENSE 
 */
#pragma once

#include <string>
#include <unordered_map>

namespace Theoria { namespace Util {


template <int ns>
class TextSpace
{
private:

    static std::unordered_map<uint32_t, std::string> s_tokenspace ;
    static const int s_ns = ns ;

public:

    explicit TextSpace(char delim = ' ') :
        _delim(delim) {} ;
    
    explicit TextSpace(const std::string& text, delim = ' ') :
        _delim(delim) {encode(text);}
    
    operator std::string () const {return decode(); }
    
    size_t size() const ; 
    
private:

    void encode(const std::string& text) ;

    char _delim ;
    std::vector<unsigned char> _tokens ;
} ;


