/**
 * Theoria - A C++ Application Container for the Real and Virtual World
 *
 * Copyright (c) 2016 theoriaacpp.com
 * 
 * This library is released under the Apache License v. 2.0. See LICENSE in top level directory of this project.
 * or https://github.com/smangano/theoria/blob/master/LICENSE 
 */

#pragma once

namespace theoria { namespace core {

class Theoria
{
public:

    Theoria() ;

    /*
     * Display theoria help and exit
     */
    void help() ;

    /*
     * Resolve the apps config, output the result 
     * Will exit if --show_config_only
     */
    void show_config(bool exit) ;

    /*
     * Initialize the Theoria container by processing the bootstrap.toml file.
     */
    void init() ;

    /*
     * Run user's application as defined by their config file
     */
    void run() ;
} ;

}}

