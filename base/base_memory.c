internal force_inline U8 mem_match(void* mem1, void* mem2, U64 size) {
    return (memcmp(mem1, mem2, size) == 0);
}

internal force_inline void mem_copy(void* dest, void* src, U64 size) {
    memcpy(dest, src, size);
}

internal force_inline void mem_set(void* mem, U8 byte, U64 size) {
    memset(mem, byte, size);
}

internal Arena* arena_alloc_(ArenaParmas* params) {
    Arena* arena;
    U64 page_size;
    U64 reserve_size;
    U64 commit_size;
    U8 alignment;
    ArenaFlag flags;
    void* memory;

    reserve_size = params->reserve_size;
    commit_size  = params->commit_size;
    flags        = params->flags;

    if (flags & ARENA_FLAG_FORCE_ALIGNMENT) {
        alignment = flags & mask(8);

        if (alignment == 0)                 bit_unset(flags, ARENA_FLAG_FORCE_ALIGNMENT_BIT);
        else if (!num_is_pow2(alignment))   alignment = 1U << u32_msb(alignment);
    }

    page_size       = (flags & ARENA_FLAG_LARGE_PAGES)      ? system_info()->large_page_size                  : system_info()->page_size;
    reserve_size    = (reserve_size)                        ? align_up_pow2(reserve_size, page_size)         : ARENA_DEFAULT_RESERVE_SIZE;
    commit_size     = (commit_size)                         ? align_up_pow2(ARENA_HEADER_SIZE, page_size)    : ARENA_DEFAULT_COMMIT_SIZE;

    memory = mem_reserve(reserve_size, 0);
    mem_commit(memory, commit_size, 0);

    arena = (Arena*)memory;
    mem_zero(arena, ARENA_HEADER_SIZE);

    arena->reserved     = reserve_size;
    arena->committed    = commit_size;
    arena->page_size    = page_size;
    arena->cursor       = ARENA_HEADER_SIZE;
    arena->flags        = flags;

    return arena;
}

internal void arena_release(Arena *arena) {
    mem_release(arena, arena->reserved);
}

internal void* arena_push(Arena* arena, U64 size, U64 align) {
    U64 begin;
    U64 end;
    U64 commit_target;;
    U8* commit_ptr;

    align           = (arena->flags & ARENA_FLAG_FORCE_ALIGNMENT) ? arena->flags & mask(8) : align;
    begin           = align_up_pow2(arena->cursor, align);
    end             = begin + size;
    arena->cursor   = end;

    if (end > arena->committed && end <= arena->reserved) {
        commit_target   = align_up_pow2(end, (U64)arena->page_size);
        commit_ptr      = (U8*)arena + arena->committed;

        mem_commit(commit_ptr, commit_target - arena->committed, arena->flags & ARENA_FLAG_LARGE_PAGES);
        arena->committed = commit_target;
    }

    return (U8*)arena + begin;
}

internal void arena_reset(Arena *arena) {
    arena->cursor = ARENA_HEADER_SIZE;
    arena->temp_stack_head = 0;
    arena->temp_stack_tail = 0;
}

internal void arena_temp_begin(Arena *arena) {
    ArenaTemp* node;
    U64 cursor;

    cursor = arena->cursor;
    node = arena_push_type(arena, ArenaTemp);
    node->cursor = cursor;

    sll_stack_push_ht(arena->temp_stack_head, arena->temp_stack_tail, node);
}

internal void arena_temp_release(Arena *arena) {
    if (arena->temp_stack_head) {
        arena->cursor = arena->temp_stack_head->cursor;
        sll_stack_pop_ht(arena->temp_stack_head, arena->temp_stack_tail);
    }
}

internal void arena_temp_release_all(Arena* arena) {
    if (arena->temp_stack_tail)
        arena->cursor = arena->temp_stack_tail->cursor;

    arena->temp_stack_head = 0;
    arena->temp_stack_tail = 0;
}

global thread_var Arena* scratch_pool[ARENA_DEFAULT_SCRATCH_COUNT] = {0};
global thread_var U8 scratch_pool_mask = (1u << ARENA_DEFAULT_SCRATCH_COUNT) - 1;

internal Arena* arena_scratch_alloc_(ArenaParmas* params) {
    I32 index;
    Arena* arena;

    index = u32_lsp((U32)scratch_pool_mask);
    arena = 0;

    if (index >= 0 && index < ARENA_DEFAULT_SCRATCH_COUNT) {
        arena = scratch_pool[index];
        if (!arena) {
            params->flags |= ARENA_FLAG_SCRATCH;
            arena = arena_alloc_(params);
            scratch_pool[index] = arena;
        }
        arena_reset(arena);
        bit_unset(scratch_pool_mask, index);
    }

    return arena;
}

internal void arena_scratch_release(Arena* arena) {
    I32 i;

    if (arena->flags & ARENA_FLAG_SCRATCH)
        for (i = 0; i < ARENA_DEFAULT_SCRATCH_COUNT; ++i)
            if (scratch_pool[i] == arena) {
                bit_set(scratch_pool_mask, i);
                break;
            }
}