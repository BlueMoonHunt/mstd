#include "mstd.h"

#include "core/mstd_mem.c"
#include "core/mstd_string.c"

#if defined(MSTD_USE_ARENA)
    #include "arena/mstd_arena.c"
#endif  // End Arena

#if defined(MSTD_USE_OS)
    #if defined(_WIN32)
        #include "os/mstd_os_win32.c"
    #endif
#endif  // End OS