/*
 * Theoria - A C++ Application Container for the Real and Virtual World
 *
 * Copyright (c) 2016 theoriaacpp.com
 * 
 * This library is released under the Apache License v. 2.0. See LICENSE in top level directory of this project.
 * or https://github.com/smangano/theoria/blob/master/LICENSE 
 */

#pragma once

#include <stdexcept>
#include <cstdarg>

#define RUNTIME_ERROR(...) std::runtime_error(::theoria::except::format_except(__FILE__,__LINE__,__VA_ARGS__)) 

namespace theoria { namespace except {

std::string format_except(const char *file, int line, const char * fmt, ...) ;


}} //theoria::except


