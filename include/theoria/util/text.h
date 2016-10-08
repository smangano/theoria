namespace Theoria { namespace Util {


template <int ns>
class TextSpace
{
private:

    static std::unorderd_map<uint32_t, std::string> s_tokenspace ;
    static const int s_ns = ns ;

public:

    explicit TextSpace(char delim = ' ') :
        _delim(delim) {} ;
    
    explicit TextSpace(const std::string& text, delim = ' ') :
        _delim(delim) {encode(text);}
    
    operator std::string () const {return decode(); }
    
    size_t size() const ; 
    
private:

    void encode(const std::string& text) ;

    char _delim ;
    std::vector<unsigned char> _tokens ;
} ;


