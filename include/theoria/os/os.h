/*
 * Theoria - A C++ Application Container for the Real and Virtual World
 *
 * Copyright (c) 2016 theoriaacpp.com
 * 
 * This library is released under the Apache License v. 2.0. See LICENSE in top level directory of this project.
 * or https://github.com/smangano/theoria/blob/master/LICENSE 
 */

#pragma once

#include <string>
#include <vector>

namespace theoria { namespace os {

/*
 * Split a search path into it's dir components (e.g. PATH env var)
 */
std::vector<std::string> split_search_path(const std::string& path) ;

/*
 * Join two or more dir names into a filename path.
 * E.g. Linux join_path("a", "b", "c") -> "a/b/c"
 *      Win   join_path("a", "b", "c") -> "a\b\c"
 */

std::string join_path(
    const std::string& p1, 
    const std::string& p2, 
    const std::string& p3 = "",
    const std::string& p4 = "",
    const std::string& p5 = "",
    const std::string& p6 = "",
    const std::string& p7 = "",
    const std::string& p8 = "",
    const std::string& p9 = ""
    ) ;

/* Test if filename exists
 *
 */
bool exists(const std::string& filename) ;

/* Test if dirname exists
 *
 */
bool dir_exists(const std::string& dirname) ;


/* Return the OS specific path to search for bootstrap.toml when it is not provided by
 * command line option or env var
 */
std::string bootstrap_path() ;

}} 
