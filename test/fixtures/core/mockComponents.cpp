const char * const TOML_CONFIG = \
"\
[MockComponent]             \n\
kind=\"Component\"          \n\
type=\"MockComponent\"      \n\
                            \n\
[MockComponent2]            \n\
kind=\"Component\"          \n\
type=\"MockComponent\"      \n\
subtype=\"MockComponent2\"  \n\
" ;

struct MockComponent : public ::theoria::core::Component
{
    MockComponent(::theoria::core::CompId id):
        ::theoria::core::Component(id, "MockCompoent") {}

    static ::theoria::core::Component* factory(::theoria::core::CompId id) {
        return new MockComponent(id) ;
    }

    static void reg() ;    

} ;

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
} ;

void MockComponent2::reg() 
{
    ::theoria::core::RegisterThis<MockComponent2> rt("MockComponent", "MockComponent2"); 
}
