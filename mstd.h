#ifndef MSTD_IMPLEMENTATION
#define MSTD_IMPLEMENTATION

#include "core/mstd_core.h"

#if defined(MSTD_USE_ARENA)
    #ifndef MSTD_USE_OS
        #define MSTD_USE_OS
    #endif
    #include "arena/mstd_arena.h"
#endif  // End Arena

#if defined(MSTD_USE_OS)
    #include "os/mstd_os.h"
#endif  // End OS


#endif // MSTD_IMPLEMENTATION