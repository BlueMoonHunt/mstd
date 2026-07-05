internal Str8 str8_from_16(Arena* arena, Str16 str) {
    U16* ptr;
    U16* opl;
    U64 size;
    U64 at;
    Str8 result;
    UnicodeDecode d;
    U8* out;

    ptr = str.data;
    opl = ptr + str.size;
    size = 0;
    at = 0;
    result.data = 0;
    result.size = 0;

    for (; ptr < opl;) {
        d = utf16_decode(ptr, opl - ptr);
        ptr += d.inc;
        size += utf8_size(d.codepoint);
    }

    out = arena_push_array(arena, U8, size + 1);

    for (ptr = str.data; ptr < opl;) {
        d = utf16_decode(ptr, opl - ptr);
        ptr += d.inc;
        at += utf8_encode(out + at, d.codepoint);
    }

    out[size] = 0;
    result.data = out;
    result.size = size;
    return result;
}

internal Str8 str8_from_32(Arena* arena, Str32 str) {
    Str8 result;
    U32* opl;
    U32* ptr;
    U64 size;
    U64 at;
    U8* out;

    result = (Str8){0};
    size = 0;
    at = 0;
    ptr = str.data;
    opl = ptr + str.size;

    for (; ptr < opl; ptr++)
        size += utf8_size(*ptr);

    out = arena_push_array(arena, U8, size + 1);

    for (ptr = str.data; ptr < opl; ptr++)
        at += utf8_encode(out + at, *ptr);

    out[size] = 0;
    result.data = out;
    result.size = size;

    return result;
}

internal Str8 str8_from_cstr(char* str) {
    Str8 result;
    U64 size;

    for (size = 0; str[size] != 0; size++);

    result.data = (U8*)str;
    result.size = size;

    return result;
}

internal Str8 str8_from_mem_size(Arena* arena, U64 size) {
    Str8 result;

    result.data = arena_push_array(arena, U8, size + 1);
    result.data[size] = 0;
    result.size = size;

    return result;
}

internal Str8 str8_from_fmt(Arena* arena, char* fmt, ...) {
    Str8 result;
    va_list args;
    va_list args_copy;
    I32 length;

    result = (Str8){0};

    va_start(args, fmt);

    va_copy(args_copy, args);
    length = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (length) {
        result = str8_from_mem_size(arena, length);
        vsnprintf((char*)result.data, result.size + 1, fmt, args);
        result.data[result.size] = 0;
    }

    va_end(args);
    return result;
}

internal Str8 str8_from_slice(Arena* arena, Str8Slice slice) {
    Str8 result;

    result.data = arena_push_array(arena, U8, slice.size + 1);

    result.size = slice.size;
    result.data = slice.data;
    result.data[result.size] = 0;

    return result;
}

internal U32 str8_match_(Str8 a, Str8 b, Str8MatchOpt opt) {
    U32 match;
    U64 i;
    U8 ca;
    U8 cb;

    match = 0;

    if (a.size == b.size && (opt.case_insensitive || opt.slash_insensitive)) {
        for (i = 0; i < a.size; ++i) {
            ca = a.data[i];
            cb = b.data[i];
            if (opt.case_insensitive) {
                ca = char_to_lower(ca);
                cb = char_to_lower(cb);
            }
            if (opt.slash_insensitive) {
                if (char_is_slash(ca))
                    ca = '/';
                if (char_is_slash(cb))
                    cb = '/';
            }
            if (ca == cb) {
                match = 1;
                break;
            }
        }
    } else if (a.size == b.size)
        match = mem_match(a.data, b.data, a.size);

    return match;
}

internal U64 str8_find_(Str8 string, Str8 substring, U64 offset, Str8MatchOpt opt) {
    U64 result;
    U8* cursor;
    U8* stop;
    U8 c_substr;
    U8 c_str;

    result = STRNPOS;

    if (substring.size > 0 && string.size >= (substring.size + offset)) {
        stop = string.data + string.size - substring.size + 1;

        for (cursor = string.data + offset; cursor < stop; cursor++) {
            c_substr = substring.data[0];
            c_str = *cursor;

            if (opt.case_insensitive) {
                c_substr = char_to_lower(c_substr);
                c_str = char_to_lower(c_str);
            }
            if (opt.slash_insensitive) {
                if (char_is_slash(c_substr))
                    c_substr = '/';
                if (char_is_slash(c_str))
                    c_str = '/';
            }
            if (c_str == c_substr && str8_match_((Str8){.data = cursor, .size = substring.size}, substring, opt)) {
                result = (U64)(cursor - string.data);
                break;
            }
        }
    }

    return result;
}

internal U64 str8_find_reverse_(Str8 string, Str8 substring, U64 offset, Str8MatchOpt opt) {
    U64 result;
    U8* cursor;
    U8 c_substr;
    U8 c_str;

    result = STRNPOS;

    if (substring.size > 0 && string.size >= (substring.size + offset))
        for (cursor = string.data + string.size - offset - substring.size; cursor >= string.data; cursor--) {
            c_substr = substring.data[0];
            c_str = *cursor;

            if (opt.case_insensitive) {
                c_substr = char_to_lower(c_substr);
                c_str = char_to_lower(c_str);
            }
            if (opt.slash_insensitive) {
                if (char_is_slash(c_substr))
                    c_substr = '/';
                if (char_is_slash(c_str))
                    c_str = '/';
            }
            if (c_str == c_substr && str8_match_((Str8){.data = cursor, .size = substring.size}, substring, opt)) {
                result = (U64)(cursor - string.data);
                break;
            }
        }

    return result;
}

internal Str8 str8_concat(Arena* arena, Str8 a, Str8 b) {
    Str8 result = str8_from_mem_size(arena, a.size + b.size);

    mem_copy(result.data, a.data, a.size);
    mem_copy(result.data + a.size, b.data, b.size);
    result.data[result.size] = 0;

    return result;
}

internal Str8 str8_concat_n_(Arena* arena, ...) {
    va_list args;
    U8* buffer;
    U64 size;
    Str8 result;
    U64 offset;

    va_start(args, arena);

    for (size = 0; result.data; size += result.size)
        result = va_arg(args, Str8);

    va_end(args);

    buffer = arena_push_array(arena, U8, size);

    va_start(args, arena);

    for (offset = 0; result.data || result.size != STRNPOS; offset += result.size) {
        result = va_arg(args, Str8);
        mem_copy(buffer + offset, result.data, result.size);
    }

    va_end(args);

    result.size = size;
    result.data = buffer;

    return result;
}

internal Str8 str8_copy(Arena *arena, Str8 str) {
    Str8 string;

    string = str8_from_mem_size(arena, str.size);
    mem_copy_array(string.data, str.data, str.size);
    string.data[string.size] = 0;

    return string;
}

internal char *str8_copy_to_cstr(Arena *arena, Str8 str) {
    char* string;

    string = arena_push_array(arena, char, str.size + 1);
    mem_copy_array(string, str.data, str.size);
    string[str.size] = 0;

    return string;
}

internal Str8Slice str8_slice_(Str8Slice str, U64 pos, Str8SliceOpt opt) {
    U64 i;
    U8 delimiter_lookup[256];

    mem_set(delimiter_lookup, 0, sizeof(delimiter_lookup));
    pos = clamp_top(pos, str.size);

    if (opt.postfix) {
        str.data += pos;
        str.size -= pos;
    } else
        str.size = pos;

    if (opt.delimiter.size > 0) {
        for (i = 0; i < opt.delimiter.size; i++) {
            delimiter_lookup[opt.delimiter.data[i]] = 1;
        }
        for (i = 0; i < str.size; i++) {
            if (delimiter_lookup[str.data[i]]) {
                str.size = i;
                break;
            }
        }
    }

    return str;
}