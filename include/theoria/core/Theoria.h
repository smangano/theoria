/**
 * Theoria - A C++ Application Container for the Real and Virtual World
 *
 * Copyright (c) 2016 theoriaacpp.com
 * 
 * This library is released under the Apache License v. 2.0. See LICENSE in top level directory of this project.
 * or https://github.com/smangano/theoria/blob/master/LICENSE 
 */

#pragma once

#include <iostream>
#include <string>

namespace theoria { namespace core {

/**
 * Implements the top-level operation of the theoria app container
 */
class Theoria
{
public:


    /**
     * Constructor
     */
    Theoria() ;

    /**
     * Display theoria help and exit
     */
    void help(std::ostream& out = std::cout) ;

    /**
     * Display help for particular option name
     */
    void help(const std::string& optionName, std::ostream& out = std::cout) ;

    /**
     * Display help for particular option (see theroria/options.h)
     */
    void help(const char * const *opt, std::ostream& out = std::cout) ;

    /**
     * Resolve the apps config, output the result 
     * Will exit if --show_config_only
     */
    void show_config(bool exit, std::ostream& out = std::cout) ;

    /**
     * Initialize the Theoria container by processing the bootstrap.toml file.
     */
    void init() ;

    /**
     * Run user's application as defined by their config file
     */
    void run() ;
} ;

}}

