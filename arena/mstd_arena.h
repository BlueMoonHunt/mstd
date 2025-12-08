#ifndef MSTD_ARENA_IMPLEMENTATION
#define MSTD_ARENA_IMPLEMENTATION
#include <core/mstd_core.h>

typedef enum ArenaFlag ArenaFlag;
enum ArenaFlag {
    arena_flag_none = 0,
    arena_flag_allocate_large_pages = 1 << 0,
};

typedef struct ArenaCreateInfo ArenaCreateInfo;
struct ArenaCreateInfo {
    u64 reserve_size;
    u64 commit_size;
    b32 allocate_large_pages;
};

typedef struct Arena Arena;
typedef struct ArenaTemp ArenaTemp;

Arena* arena_alloc(const ArenaCreateInfo create_info);
void arena_release(Arena* arena);
void* arena_push(Arena* arena, const u64 size, const u64 alignment);
ArenaTemp* arena_temp_begin(Arena* arena);
void arena_temp_end(ArenaTemp* arena_temp);

#define arena_scratch(arena) \
    for (ArenaTemp *_temp_ = arena_temp_begin(arena); \
         _temp_ != NULL; \
         arena_temp_end(_temp_), _temp_ = NULL)

#endif // MSTD_ARENA_IMPLEMENTATION