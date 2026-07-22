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