#include <theoria/config/config.h>

Config::Config() 
{
}

Config::~Config() 
{
    _parent = nullptr ;
    for(auto child : _children) 
        delete _children ;
}


