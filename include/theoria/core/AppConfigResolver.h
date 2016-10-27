#pragma once

#include <theoria/config/Resolve.h>

namespace theoria { namespace core {

/*
 * Resolve variables from your config file at the top level
 * or nested using dot notation: E.g., $a.b
 */

class AppConfigResolver : public config::ConfigVariableResolver
{
public:

    Result lookup(const std::string& name) const override;
    virtual std::string name() const override ;
} ;


}} //theoria::core
