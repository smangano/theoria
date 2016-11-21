/*
 * Theoria - A C++ Application Container for the Real and Virtual World
 *
 * Copyright (c) 2016 theoriacpp.com
 * 
 * This library is released under the Apache License v. 2.0. See LICENSE in top level directory of this project.
 * or https://github.com/smangano/theoria/blob/master/LICENSE 
 */
#pragma once

#include <theoria/config/Resolve.h>

namespace theoria { namespace core {

/**
 * Resolve variables from your application's config file at the top level
 * or nested using dot notation: E.g., $a.b
 */

class AppConfigResolver : public config::ConfigVariableResolver
{
public:

    /**
     * Look up name in app config
     */
    Result lookup(const std::string& name) const override;

    /**
     * Returns "AppConfigResolver"
     */
    virtual std::string name() const override ;
} ;


}} //theoria::core
