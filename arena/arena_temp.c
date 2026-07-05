internal void arena_temp_push(Arena *arena) {
    ArenaTempNode* node;

    node = arena_push_type(arena, ArenaTempNode);
    sll_stack_push(arena->temp_stack_head, arena->temp_stack_tail, node);
}

internal void arena_temp_pop(Arena *arena) {
    U64 cursor;

    if (arena->temp_stack_head) {
        cursor = (U64)((U8 *)arena->temp_stack_head - (U8 *)arena);
        sll_stack_pop(arena->temp_stack_head, arena->temp_stack_tail);
        arena->cursor = cursor;
    }
}

internal void arena_temp_pop_all(Arena* arena) {
    if (arena->temp_stack_tail)
        arena->cursor = (U64)((U8*)arena->temp_stack_tail - (U8*)arena);
    else
        arena->cursor = ARENA_HEADER_SIZE;

    arena->temp_stack_head = 0;
    arena->temp_stack_tail = 0;
}