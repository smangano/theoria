#include <theoria/core/Component.h>

using namespace theoria ;
using namespace core ;

Component::Component() 
    : _id(-1), _name("")
{
}

Component::~Component() {}

Dependencies Component::init(const config::Config& config) 
{
    return Dependencies();
}

void Component::finalize(std::vector<Component*>& dependencies) 
{
}

void Component::appLifeCycle(AppLifeCycle state) 
{
}

void Component::compLifeCycle(CompLifeCycle state, CompId id) 
{
}

Component* Component::acquire(const std::type_info& typeInfo, void** dest) 
{
    *dest = nullptr ;
    return nullptr ;
}

