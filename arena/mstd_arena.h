#ifndef MSTD_ARENA_IMPLEMENTATION
#define MSTD_ARENA_IMPLEMENTATION
#include <core/mstd_types.h>

typedef struct ArenaCreateInfo ArenaCreateInfo;
struct ArenaCreateInfo {
    u64 reserve_size;
    u64 commit_size;
    b32 allocate_large_pages;
};

typedef struct Arena Arena;
struct Arena {
    u64 cmt_size;
    u64 res_size;
    u64 base_pos;
    u64 pos;
    u64 cmt;
    u64 res;
};

#endif // MSTD_ARENA_IMPLEMENTATION