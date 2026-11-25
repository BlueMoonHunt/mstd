#if !defined(MSTD_H)
#define MSTD_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Winitializer-overrides"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Woverride-init"
#endif

#include "base/base.h"
#include "gfx/gfx.h"

#if MSTD_USE_MAIN /* forces uses of only "int main(int argc, char* argv[])" startup */

#if MSTD_NO_GFX
#define mstd_gfx_init() ((void)0)
#else
#define mstd_gfx_init() gfx_init()
#endif

int mstd_main(int argc, char* argv[]);

#define main(...)                       \
    main(int argc, char* argv[]) {      \
        int result;                     \
        base_init();                    \
        mstd_gfx_init();                \
        result = mstd_main(argc, argv); \
        return result;                  \
    }                                   \
    int mstd_main(__VA_ARGS__)

#endif

#endif /* MSTD_H */