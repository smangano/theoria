#include <theoria/core/Component.h>

using namespace theoria ;
using namespace core ;

Component::~Component() {}

Dependencies Component::init(const config::Config& config) 
{
    return Dependencies();
}

void Component::finalize(std::vector<Component*> dependencies) 
{
}
