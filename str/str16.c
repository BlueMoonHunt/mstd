internal Str16 str16_from_cstr(U16* str) {
    U64 size;
    Str16 result;

    for (size = 0; str[size] != 0; size++);

    result.data = str;
    result.size = size;

    return result;
}

internal Str16 str16_from_8(Arena* arena, Str8 str) {
    Str16 result;
    U8 *ptr;
    U8 *opl;
    U16 *out;
    U64 size;
    U64 at;
    UnicodeDecode d;

    result = (Str16){0};

    if (str.size) {
        ptr = str.data;
        opl = ptr + str.size;

        for (size = 0; ptr < opl;) {
            d = utf8_decode(ptr, opl - ptr);
            ptr += d.inc;
            size += utf16_size(d.codepoint);
        }

        out = arena_push_array(arena, U16, size + 1);

        ptr = str.data;

        for (at = 0; ptr < opl;) {
            d = utf8_decode(ptr, opl - ptr);
            ptr += d.inc;
            at += utf16_encode(out + at, d.codepoint);
        }

        out[size] = 0;
        result.data = out;
        result.size = size;
    }

    return result;
}

internal Str16 str16_from_mem_size(Arena* arena, U64 size) {
    Str16 result;

    result.data = arena_push_array(arena, U16, size + 1);
    result.data[size] = 0;
    result.size = size;

    return result;
}