#include <theoria/util/CommandLine.h>

#include <iostream>
#include <stdexcept>

using namespace theoria ;

int main(int argc, const char ** argv) 
{
    try {
        util::CommandLine(argc-1, &(argv[1])) ;

    }

    catch (const std::runtime_error& ex) {
        std::cerr << ex.what() <<std::endl ;
    }
}
