#pragma once

#include <string>
#include <istream>

namespace theoria { namespace config {

class Config ;

class ConfigParser
{
public:

    virtual ~ConfigParser() {};

    /**
     * Default implementation opens the file and calls
     * parse(with the file's std::ifstream)
     *
     * @filename the file to parse
     * @return the resulting Config object
     *
     * NOTE:: If you override parse_file in your derived 
     *        ConfigParser then you can optionally implement 
     *        parse or simply stub it out as Theoria will only call
     *        parse_file directly. A stream-based interface is useful
     *        for writing unit tests using istringstream.
     */
    virtual Config* parse_file(const std::string& filename) ;

    /* 
     * Parse the stream and return a Config
     */
    virtual Config* parse(std::istream& stream)  = 0 ;
} ;

}}
