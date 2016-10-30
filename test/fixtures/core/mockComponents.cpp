const char * const TOML_CONFIG = \
"\
[MockComponent]             \n\
kind=\"Component\"          \n\
type=\"MockComponent\"      \n\
subtype=\"MockComponent\"   \n\
                            \n\
[MockComponent2]            \n\
kind=\"Component\"          \n\
type=\"MockComponent\"      \n\
subtype=\"MockComponent2\"  \n\
" ;

const char * const TOML_CONFIG1 = \
"\
[MockComponent]             \n\
kind=\"Component\"          \n\
type=\"MockComponent\"      \n\
" ;

struct MockComponent : public ::theoria::core::Component
{
    MockComponent(::theoria::core::CompId id):
        ::theoria::core::Component(id, "MockCompoent") {}

    static ::theoria::core::Component* factory(::theoria::core::CompId id) {
        return new MockComponent(id) ;
    }

    static void reg() ;    

    void finalize(std::vector<::theoria::core::Component*>& dependencies) override ;

    std::vector<Component*> myDeps ;
} ;

void MockComponent::finalize(std::vector<Component*>& dependencies)
{
    myDeps = dependencies ;
}

void MockComponent::reg() {
    ::theoria::core::RegisterThis<MockComponent> rt("MockComponent") ;
}    

struct MockComponent2 : public MockComponent
{
    MockComponent2(::theoria::core::CompId id):
        MockComponent(id) {_name = "MockComponent2";}

    static ::theoria::core::Component* factory(::theoria::core::CompId id) {
        return new MockComponent2(id) ;
    }

    static void reg() ;     

    ::theoria::core::Dependencies init(const ::theoria::config::Config& config) override ;
} ;

void MockComponent2::reg() 
{
    ::theoria::core::RegisterThis<MockComponent2> rt("MockComponent", "MockComponent2"); 
}

::theoria::core::Dependencies MockComponent2::init(const ::theoria::config::Config& config)
{
    return ::theoria::core::Dependencies().strict("MockComponent", "") ;
}

