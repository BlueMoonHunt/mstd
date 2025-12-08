#include "mstd_arena.h"
#include <os/mstd_os.h>

typedef struct Arena Arena;
struct Arena {
    uaddress base;
    u64 commited;
    u64 commit_size;
    u64 cursor;
    enum_class(ArenaFlag, u32) flags;
    u64 reserved;
};

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
    arena->base = (uaddress)base;
    arena->commit_size = commit_size;
    arena->commited = commit_size;
    arena->cursor = 0;
    arena->flags = (create_info.allocate_large_pages) ? arena_flag_allocate_large_pages : arena_flag_none;
    arena->reserved = reserve_size;

    return arena;
}

void arena_release(Arena* arena) {
    os_release(arena, arena->reserved);
}

void* arena_push(Arena* arena, const u64 size, const u64 alignment) {
    u64 begin_pos = align_up_pow2(arena->cursor, alignment);
    u64 end_pos = begin_pos + size;

    if (arena->commited < end_pos) {
        b32 commit_success = 0;
        void* commit_ptr = (void*)(arena->base + arena->commited);
        u64 commit_size = min(align_up_pow2(end_pos, arena->commit_size), arena->reserved) - arena->commited;

        if (arena->flags & arena_flag_allocate_large_pages)
            commit_size = os_commit_large(commit_ptr, commit_size);
        else
            commit_size = os_commit(commit_ptr, commit_size);

        if (!commit_success)
            arena->commited += commit_size;
        }

    void* result = 0;
    if(arena->commited >= end_pos){
        result = (void*)(arena->base + begin_pos);
        arena->cursor = end_pos;
    }

    return result;
}

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp {
    Arena* arena;
    u64 cursor;
};

ArenaTemp* arena_temp_begin(Arena* arena) {
    assert(arena);
    const u64 cursor = arena->cursor;

    ArenaTemp* temp = (ArenaTemp*)arena_push(arena, sizeof(ArenaTemp), alignof(ArenaTemp));
    temp->arena = arena;
    temp->cursor = cursor;

    return temp;
}

void arena_temp_end(ArenaTemp* arena_temp) {
    assert(arena_temp);
    arena_temp->arena->cursor = arena_temp->cursor;
}