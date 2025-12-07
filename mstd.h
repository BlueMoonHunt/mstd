#ifndef MSTD_IMPLEMENTATION
#define MSTD_IMPLEMENTATION

#include "core/mstd_types.h"
#include "core/mstd_mem.h"

#if defined(__INTELLISENSE__) || defined(__RESHARPER__) || defined(__clang_analyzer__)
    #define MSTD_USE_OS
    #define MSTD_USE_ARENA
    #define MSTD_USE_MEMORY
#endif


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