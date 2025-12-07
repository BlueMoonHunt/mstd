#include "mstd_arena.h"
#include <os/mstd_os.h>

Arena* arena_alloc(const ArenaCreateInfo create_info) {
    u64 reserve_size = create_info.reserve_size;
    u64 commit_size = create_info.commit_size;

    if(create_info.allocate_large_pages) {
        reserve_size = align_up_pow2(reserve_size, os_get_large_page_size());
        commit_size = align_up_pow2(commit_size, os_get_large_page_size());
    }
    else {
        reserve_size = align_up_pow2(reserve_size, os_get_page_size());
        commit_size = align_up_pow2(commit_size, os_get_page_size());
    }

    void* base;
    if(create_info.allocate_large_pages) {
        base = os_reserve_large(reserve_size);
        os_commit_large(base, commit_size);
    }
    else {
        base = os_reserve(reserve_size);
        os_commit(base, commit_size);
    }

    Arena* arena = (Arena*)base;
    arena->reserve_size = reserve_size;
    arena->commit_size = commit_size;
    arena->base = (Arena*)base + 1;
    arena->offset = 0;

    return arena;
}

void arena_release(Arena* arena) {
    os_release(arena, arena->reserve_size);
}