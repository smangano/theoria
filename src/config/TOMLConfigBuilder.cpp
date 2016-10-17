#include <theoria/config/TOMLConfigBuilder.h>
#include <theoria/config/Config.h>
#include <theoria/except/except.h>
#include <cpptoml.h>

using namespace theoria ;
using namespace config ;

Config* TOMLConfigBuilder::parse(std::istream& stream)
{
    try {
        cpptoml::parser parser_(stream) ;
        auto config = parser_.parse();
        auto application = config->get_as<std::string>("application").value_or("") ;
        auto appdesc = config->get_as<std::string>("desc").value_or("") ;
        pushConfig(application, appdesc) ;
        _recursive_build(*config) ;
    }
    catch(const cpptoml::parse_exception& parseEx) {
        throw RUNTIME_ERROR("Could not parse TOML config: %s", parseEx.what()) ;
    }

    return releaseAll() ;
}

Config* TOMLConfigBuilder::parse_file(const std::string& filename)
{
    try {
        auto config = cpptoml::parse_file(filename);
        if (!config)
            throw RUNTIME_ERROR("Could not parse TOML config: %s", filename.c_str()) ;

        auto application = config->get_as<std::string>("application").value_or(filename) ;
        auto appdesc = config->get_as<std::string>("desc").value_or("") ;
        pushConfig(application, appdesc) ;
        _recursive_build(*config) ;
    }
    catch(const cpptoml::parse_exception& parseEx) {
        throw RUNTIME_ERROR("Could not parse TOML config: %s: %s", filename.c_str(), parseEx.what()) ;
    }

    return releaseAll() ;
}

void TOMLConfigBuilder::_recursive_build(cpptoml::table& table)
{
    for (auto iter = table.begin(), end = table.end(); iter != end; ++iter)
    {
        if (iter->second->is_value()) 
        {
            if (iter->first == "desc")
               setDesc(*table.get_as<std::string>(iter->first)) ;
            else {
                //attributes
                auto val_type = _getValueAndTypeAsString(table, iter->first) ;
                auto value = val_type.first ;
                auto type = val_type.second ;
                if (value != "" && value[0] == '$')
                    _addAttrWithResolve(iter->first, value, type) ;
                else {
                    _addAttr(iter->first, value, type) ;
                }
            }
        }
        else
        if (iter->second->is_array())
        {
            pushConfigArray(iter->first) ;
            auto tarr = table.get_table_array(iter->first);
            for (const auto& tableElem : *tarr) {
                _recursive_build(*tableElem) ;
            }
            popAsChild() ;
        }
        else
        if (iter->second->is_table()) 
        {
            pushConfig(iter->first, iter->first) ;
            _recursive_build(*(iter->second->as_table())) ;
            popAsChild() ;
        }
    }    
}


void TOMLConfigBuilder::_addAttr(const std::string& name, const std::string& value, const std::string& type)
{
     
    addAttr(name, value, type) ;
}

void TOMLConfigBuilder::_addAttrWithResolve(
    const std::string& name, 
    const std::string& value,
    const std::string& type)
{
    //TODO
}

std::pair<std::string, std::string> TOMLConfigBuilder::_getValueAndTypeAsString(const cpptoml::table& owner,const std::string& name)
{
    std::ostringstream oss ;
    auto strVal = owner.get_as<std::string>(name) ; 
    if (strVal) {
        return std::make_pair(*strVal, std::string("string")) ;
    }
    auto intVal = owner.get_as<int64_t>(name) ; 
    if (intVal) {
        oss << *intVal ; 
        return std::make_pair(oss.str(), std::string("int")) ;
    }
    auto doubleVal = owner.get_as<double>(name) ; 
    if (doubleVal) {
        oss << *doubleVal ; 
        return std::make_pair(oss.str(), std::string("double")) ;
    }
    auto boolVal = owner.get_as<bool>(name) ;
    if (boolVal) {
        oss << *boolVal ; 
        return std::make_pair(oss.str(), std::string("bool")) ;
    }
    auto dateVal = owner.get_as<cpptoml::datetime>(name) ; 
    if (dateVal)
    {
        auto dt = *dateVal ;
        oss << std::setfill('0') ;
        oss << std::setw(4) << dt.year << "-" << std::setw(2) << dt.month << "-" << std::setw(2) << dt.day ;
        if (dt.hour || dt.minute || dt.second || dt.microsecond)
        {
            oss << "T" << std::setw(2) << dt.hour << ":" << std::setw(2)  << dt.minute << ":" << std::setw(2) 
                << dt.second ;
            if (dt.microsecond) 
                oss << std::setw(6) << dt.microsecond ;
            if (dt.hour_offset || dt.minute_offset) {
                if (dt.hour_offset < 0) {
                    oss << "-" ;
                    dt.hour_offset *= -1 ;
                }
                oss << std::setw(2) << dt.hour_offset << ":" << std::setw(2) << dt.minute_offset ;
            }
            return std::make_pair(oss.str(), "datetime") ;
        }
        else {
            return std::make_pair(oss.str(),"date") ;
        }
    }
    return std::make_pair("" , "") ;
}
