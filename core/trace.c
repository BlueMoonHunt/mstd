internal void trace_(TraceLevel level, Str8 message) {
    Arena* arena;
    arena = arena_scratch_begin();

    char* level_str[4] = { "INFO", "WARNING", "ERROR", "UNKNOWN_TRACE_LEVEL" };

    if (level > 2)
        level = 3;

    message = str8_from_fmt(arena, "%s: %s", level_str[level], message);

    if (trace_file.val[0])
        file_write_string(trace_file, message);

    if (!trace_is_hidden)
        printf("%s\n", message.data);

    arena_scratch_end(arena);
}