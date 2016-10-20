#include <theoria/except/except.h>

#include <boost/algorithm/string/predicate.hpp>

namespace theoria { namespace util {

/* Convert a string to a T
 * T must be something that can be read from a stream or you will get a compile error
 *
 * Examples:
 *  convert<unsigned int>("555") 
 */

template <typename T1>
inline
const char * value(const std::pair<T1, std::string>& pair) {return pair.second.c_str() ;}

template <typename T>
inline
const char * value(const T& valued) {return valued.value.c_str() ;}

template <typename T>
T convert(const std::string & val)
{
    T result ;
    std::istringstream iss(val) ;
    iss >> result ;
    return result ;  
}

template <typename TIter>
int convertToInt(TIter target, TIter end, const char * context, const std::string& name, int def = 0 )
{
    if (target != end)
    {
        char * err = nullptr;
        int64_t result = strtoll(value(*target), &err, 10)  ;
        if (err && *err)
            throw RUNTIME_ERROR("%sAsInt(%s, %lld) %s is not an integer", context, name.c_str(), def, value(*target)) ;
        return result ;
    }
    return def ;
}

template <typename TIter>
double convertToDbl(TIter target, TIter end, const char * context, const std::string& name, double def = 0.0 )
{
    if (target != end)
    {
        char * err = nullptr;
        double result = strtod(value(*target), &err)  ;
        if (err && *err)
            throw RUNTIME_ERROR("%sAsDbl(%s, %g) %s is not a double", context, name.c_str(), def, value(*target)) ;
        return result ;
    }
    return def ;
}

template <typename TIter>
bool convertToBool(TIter target, TIter end, const char * context, const std::string& name, bool def = false )
{
    if (target != end)
    {
        const char * v = value(*target) ;
        if ( boost::iequals(v, "false") || strcmp(v, "0") == 0 )
            return false ;
        if ( boost::iequals(v, "true") || strcmp(v, "1") == 0 )
            return true ;
        throw RUNTIME_ERROR("%sAsBool(%s, %s) %s is not a bool", context, name.c_str(), (def ? "true" : "false"), v) ;
    }
    return def ;
}

}}
