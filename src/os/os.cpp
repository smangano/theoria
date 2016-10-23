#include <theoria/os/os.h>

#if defined(__linux__)
#include "./linux/os.cpp"
#else
#error OS NOT YET SUPPORTED
#endif

