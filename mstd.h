#if !defined(MSTD_H)
#define MSTD_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Winitializer-overrides"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Woverride-init"
#endif

#include "base/base.h"

#if MSTD_USE_MAIN /* frces uses of only "int main(int argc, char* argv[])" startup */

int mstd_main(int argc, char* argv[]);

#define main(...)                       \
    main(int argc, char* argv[]) {      \
        int result;                     \
        os_state_init();                \
        result = mstd_main(argc, argv); \
        return result;                  \
    }                                   \
    int mstd_main(__VA_ARGS__)

#endif

#endif /* MSTD_H */