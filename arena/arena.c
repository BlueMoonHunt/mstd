#include <mstd.h>

internal Arena* arena_alloc_(ArenaAllocOpt opt) {
    Arena* arena;
    U64 page_size;
    void* memory;

    page_size = (opt.large_pages) ? os_get_system_info()->large_page_size : os_get_system_info()->page_size;

    opt.reserve_size = (opt.reserve_size) ? align_up_pow2(opt.reserve_size, page_size) : ARENA_DEFAULT_RESERVE_SIZE;
    opt.commit_size = (opt.commit_size) ? align_up_pow2(ARENA_HEADER_SIZE, page_size) : ARENA_DEFAULT_COMMIT_SIZE;

    memory = os_memory_reserve(opt.reserve_size, opt.large_pages);
    os_memory_commit(memory, opt.commit_size, opt.large_pages);

    arena = (Arena *)memory;
    arena->reserved                 = opt.reserve_size;
    arena->committed                = opt.commit_size;
    arena->page_size                = page_size;
    arena->cursor                   = ARENA_HEADER_SIZE;
    arena->can_commit_large_pages   = opt.large_pages;
    arena->temp_stack_head          = 0;
    arena->temp_stack_tail          = 0;

    return arena;
}

internal void arena_release(Arena *arena) {
    os_memory_release(arena, arena->reserved);
}

internal void* arena_push(Arena* arena, U64 size, U64 align) {
    U64 begin;
    U64 end;
    U64 commit_target;;
    U8* commit_ptr;
    void* user_ptr;

    begin = align_up_pow2(arena->cursor, align);
    end = begin + size;

    if (end > arena->committed && end <= arena->reserved) {
        commit_target = align_up_pow2(end, (U64)arena->page_size);
        commit_ptr = (U8*)arena + arena->committed;
        os_memory_commit(commit_ptr, commit_target - arena->committed, arena->can_commit_large_pages);
        arena->committed = commit_target;
    }

    user_ptr = (U8*)arena + begin;
    arena->cursor = end;

    return user_ptr;
}

internal void arena_reset(Arena *arena) {
    arena->cursor = ARENA_HEADER_SIZE;
    arena->temp_stack_head = 0;
    arena->temp_stack_tail = 0;
}