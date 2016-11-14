#pragma once

#include <string>
#include <istream>
#include <memory>

namespace theoria { namespace config {

class Config ;

/**
 * Interface for parsing a config file or text in a stream. 
 * Parser implementations are responsible for converting some structured textual format (e.g., TOML, XML, JSON, etc.)
 * and converting it into Config representation.
 */
class ConfigParser
{
public:

    virtual ~ConfigParser() {};

    /**
     * Default implementation opens the file and calls
     * parse(with the file's std::ifstream)
     *
     * @param filename the file to parse
     * @return the resulting Config object
     *
     * NOTE:: If you override parse_file in your derived 
     *        ConfigParser then you can optionally implement 
     *        parse or simply stub it out as Theoria will only call
     *        parse_file directly. A stream-based interface is useful
     *        for writing unit tests using istringstream.
     */
    virtual std::unique_ptr<const Config> parse_file(const std::string& filename) ;

    /** 
     * Parse the stream and return a Config
     * @param stream  an input stream to parse
     */
    virtual std::unique_ptr<const Config> parse(std::istream& stream)  = 0 ;
} ;

}}
