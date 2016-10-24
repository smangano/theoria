#include <theoria/core/Component.h>

using namespace theoria ;
using namespace core ;

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

Component* Component::bind(const std::type_info& typeInfo, void** dest) 
{
    *dest = nullptr ;
    return nullptr ;
}

