internal Str8 str8_from_str16(Arena* arena, Str16 str) {
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

internal Str8 str8_from_str32(Arena* arena, Str32 str) {
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

internal Str8 str8_from_size(Arena* arena, U64 size) {
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
        result = str8_from_size(arena, length);
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

internal U32 str8_match(Str8 a, Str8 b, enum_val(Str8MatchFlag, U32) flags) {
    U32 match;
    U64 i;
    U8 ca;
    U8 cb;

    match = 0;

    if (a.size == b.size && flags) {
        for (i = 0; i < a.size; ++i) {
            ca = a.data[i];
            cb = b.data[i];
            if (flags & STR8_MATCH_FLAG_CASE_INSENSITIVE) {
                ca = char_to_lower(ca);
                cb = char_to_lower(cb);
            }
            if (flags & STR8_MATCH_FLAG_SLASH_INSENSITIVE) {
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

internal U64 str8_find(Str8 target, Str8 query, U64 offset, enum_val(Str8MatchFlag, U32) flags) {
    U64 result;
    U8* cursor;
    U8* stop;
    U8 c1;
    U8 c2;
    Str8 sub_str;

    result = STRNPOS;

    if (query.size > 0 && target.size >= (query.size + offset)) {
        stop = target.data + target.size - query.size + 1;

        for (cursor = target.data + offset; cursor < stop; cursor++) {
            c1 = query.data[0];
            c2 = *cursor;

            if (flags & STR8_MATCH_FLAG_CASE_INSENSITIVE) {
                c1 = char_to_lower(c1);
                c2 = char_to_lower(c2);
            }
            if (flags & STR8_MATCH_FLAG_SLASH_INSENSITIVE) {
                if (char_is_slash(c1))
                    c1 = '/';
                if (char_is_slash(c2))
                    c2 = '/';
            }

            sub_str.data = cursor;
            sub_str.size = query.size;

            if (c2 == c1 && str8_match(sub_str, query, flags)) {
                result = (U64)(cursor - target.data);
                break;
            }
        }
    }

    return result;
}

internal U64 str8_find_reverse(Str8 target, Str8 query, U64 offset, enum_val(Str8MatchFlag, U32) flags) {
    U64 result;
    U8* cursor;
    U8 c1;
    U8 c2;
    Str8 sub_str;

    result = STRNPOS;

    if (query.size > 0 && target.size >= (query.size + offset))
        for (cursor = target.data + target.size - offset - query.size; cursor >= target.data; cursor--) {
            c1 = query.data[0];
            c2 = *cursor;

            if (flags & STR8_MATCH_FLAG_CASE_INSENSITIVE) {
                c1 = char_to_lower(c1);
                c2 = char_to_lower(c2);
            }
            if (flags & STR8_MATCH_FLAG_SLASH_INSENSITIVE) {
                if (char_is_slash(c1))
                    c1 = '/';
                if (char_is_slash(c2))
                    c2 = '/';
            }

            sub_str.data = cursor;
            sub_str.size = query.size;

            if (c2 == c1 && str8_match(sub_str, query, flags)) {
                result = (U64)(cursor - target.data);
                break;
            }
        }

    return result;
}

internal Str8 str8_concat(Arena* arena, Str8 a, Str8 b) {
    Str8 result = str8_from_size(arena, a.size + b.size);

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
    Str8 target;

    target = str8_from_size(arena, str.size);
    mem_copy_array(target.data, str.data, str.size);
    target.data[target.size] = 0;

    return target;
}

internal char *str8_copy_to_cstr(Arena *arena, Str8 str) {
    char* target;

    target = arena_push_array(arena, char, str.size + 1);
    mem_copy_array(target, str.data, str.size);
    target[str.size] = 0;

    return target;
}

internal Str8Slice str8_slice(Str8Slice slice, U64 begin, U64 end) {
    Str8Slice result;

    mem_set(&result, 0, sizeof result);

    if (begin <= end && end <= slice.size) {
        result.data = slice.data + begin;
        result.size = end - begin;
    }

    return result;
}