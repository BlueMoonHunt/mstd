#ifndef MSTD_ARENA_IMPLEMENTATION
#define MSTD_ARENA_IMPLEMENTATION
#include <core/mstd_core.h>

typedef struct ArenaCreateInfo ArenaCreateInfo;
struct ArenaCreateInfo {
    u64 reserve_size;
    u64 commit_size;
    b32 allocate_large_pages;
};

typedef struct Arena Arena;
struct Arena {
    u64 reserve_size;
    u64 commit_size;
    size_t base;
    size_t offset;
};


Arena* arena_alloc(const ArenaCreateInfo create_info);
void arena_release(Arena* arena);
#endif // MSTD_ARENA_IMPLEMENTATION