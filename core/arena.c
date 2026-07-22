internal Arena* arena_alloc(U64 reserve_size, U64 commit_size, U8 enable_large_pages) {
    Arena* arena;
    U64 page_size;
    void* memory;

    if (!os_state.system_info.page_size || !os_state.system_info.large_page_size)
        os_state_init();

    page_size = (enable_large_pages) ? os_get_system_info()->large_page_size : os_get_system_info()->page_size;

    reserve_size = (reserve_size) ? align_up_pow2(reserve_size, page_size) : ARENA_DEFAULT_RESERVE_SIZE;
    commit_size = (commit_size) ? align_up_pow2(ARENA_HEADER_SIZE, page_size) : ARENA_DEFAULT_COMMIT_SIZE;

    memory = os_memory_reserve(reserve_size, enable_large_pages);
    os_memory_commit(memory, commit_size, enable_large_pages);

    arena = (Arena *)memory;
    arena->reserved                 = reserve_size;
    arena->committed                = commit_size;
    arena->page_size                = page_size;
    arena->cursor                   = ARENA_HEADER_SIZE;
    arena->can_commit_large_pages   = enable_large_pages;
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

internal void arena_temp_begin(Arena *arena) {
    ArenaTempNode* node;
    U64 cursor;

    cursor = arena->cursor;
    node = arena_push_type(arena, ArenaTempNode);
    node->cursor = cursor;

    sll_stack_push(arena->temp_stack_head, arena->temp_stack_tail, node);
}

internal void arena_temp_end(Arena *arena) {
    if (arena->temp_stack_head) {
        arena->cursor = arena->temp_stack_head->cursor;
        sll_stack_pop(arena->temp_stack_head, arena->temp_stack_tail);
    }
}

internal void arena_temp_end_all(Arena* arena) {
    if (arena->temp_stack_tail)
        arena->cursor = arena->temp_stack_tail->cursor;

    arena->temp_stack_head = 0;
    arena->temp_stack_tail = 0;
}

global thread_var Arena* scratch_pool[ARENA_DEFAULT_SCRATCH_COUNT] = {0};
global thread_var U8 scratch_pool_mask = (1u << ARENA_DEFAULT_SCRATCH_COUNT) - 1;

internal Arena* arena_scratch_begin(void) {
    I32 index;
    Arena* arena = 0;

    index = u32_lsb((U32)scratch_pool_mask);

    if (index >= 0 && index < ARENA_DEFAULT_SCRATCH_COUNT) {
        arena = scratch_pool[index];
        if (!arena) {
            arena = arena_alloc_default();
            scratch_pool[index] = arena;
        }
        arena_reset(arena);
        bit_unset(scratch_pool_mask, index);
    }

    return arena;
}

internal void arena_scratch_end(Arena* arena) {
    I32 i;

    for (i = 0; i < ARENA_DEFAULT_SCRATCH_COUNT; ++i) {
        if (scratch_pool[i] == arena) {
            bit_set(scratch_pool_mask, i);
            break;
        }
    }
}