/*
 * Theoria - A C++ Application Container for the Real and Virtual World
 *
 * Copyright (c) 2016 theoriacpp.com
 * 
 * This library is released under the Apache License v. 2.0. See LICENSE in top level directory of this project.
 * or https://github.com/smangano/theoria/blob/master/LICENSE 
 */

#pragma once

/**
 * @Deprecated
 * Builds a chain of resolvers. Used by Config Builder
 */
class ConfigResolverBuilder : public core::Component 
{
public:

    ConfigResolverBuilder() {}

    /* Reads list of <theoria::config::ConfigVariableResolver>'s to determine the 
     * resolver list returned by <build>
     * @inherit
     */
	Dependencies init(const Config& config) overide;
   
    /**
     * Receives a ConfigBuilder  and one or more resolvers and wires the resolvers as 
     * the builders resolver chain in the order they appear
     */
    void finalize(ComponentList dependencies) override ;
    config::ConfigVariableResolver* build() const {return _resolvers;}

private:

    config::ConfigVariableResolver * _resolvers ;
    
} ;
