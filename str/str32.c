internal Str32 str32_from_cstr(U32* str) {
    U64 size;
    Str32 result;

    for (size = 0; str[size] != 0; size++);

    result.data = str;
    result.size = size;

    return result;
}

internal Str32 str32_from_str8(Arena* arena, Str8 str) {
    Str32 result;
    U8* ptr;
    U8* opl;
    U32* out;
    U64 size;
    U64 at;
    UnicodeDecode d;

    result = (Str32){0};

    if (str.size) {
        ptr = str.data;
        opl = ptr + str.size;

        for (size = 0; ptr < opl; size += 1) {
            d = utf8_decode(ptr, opl - ptr);
            ptr += d.inc;
        }

        out = arena_push_array(arena, U32, size + 1);

        ptr = str.data;

        for (at = 0; ptr < opl;) {
            d = utf8_decode(ptr, opl - ptr);
            ptr += d.inc;
            out[at++] = d.codepoint;
        }

        out[size] = 0;
        result.data = out;
        result.size = size;
    }

    return result;
}

internal Str32 str32_from_mem_size(Arena* arena, U64 size) {
    Str32 result;

    result.data = arena_push_array(arena, U32, size + 1);
    result.data[size] = 0;
    result.size = size;

    return result;
}