#pragma once

#include <string>
#include <vector>

namespace theoria { namespace core {

class Component ;
using ComponentList = std::vector<Component*> ;
using CompId = int ;
using ComponentFactory = Component* (*)(CompId id) ;
using Text = std::string ;
using TypeName = std::string ; 
using TypeNameList = std::vector<TypeName> ;

}} //namespace theoria::core

