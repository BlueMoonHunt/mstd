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