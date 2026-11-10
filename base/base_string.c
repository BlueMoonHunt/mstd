global read_only U8 utf8_class[32] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 5,
};

internal UnicodeDecode utf8_decode(U8* str, U64 max) {
    UnicodeDecode result;
    U8 byte;
    U8 byte_class;
    U8 cont_byte[3];

    result.inc = 1;
    result.codepoint = U32_MAX;

    byte = str[0];
    byte_class = utf8_class[byte >> 3];

    switch (byte_class) {
    case 1: {
        result.codepoint = byte;
    } break;
    case 2: {
        if (1 < max) {
            cont_byte[0] = str[1];
            if (utf8_class[cont_byte[0] >> 3] == 0) {
                result.codepoint = (byte & 0x1f) << 6;
                result.codepoint |= (cont_byte[0] & 0x3f);
                result.inc = 2;
            }
        }
    } break;
    case 3: {
        if (2 < max) {
            cont_byte[0] = str[1];
            cont_byte[1] = str[2];
            if (utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0) {
                result.codepoint = (byte & 0xf) << 12;
                result.codepoint |= ((cont_byte[0] & 0x3f) << 6);
                result.codepoint |= (cont_byte[1] & 0x3f);
                result.inc = 3;
            }
        }
    } break;
    case 4: {
        if (3 < max) {
            cont_byte[0] = str[1];
            cont_byte[1] = str[2];
            cont_byte[2] = str[3];
            if (utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0 &&
                utf8_class[cont_byte[2] >> 3] == 0) {
                result.codepoint = (byte & 0x7) << 18;
                result.codepoint |= ((cont_byte[0] & 0x3f) << 12);
                result.codepoint |= ((cont_byte[1] & 0x3f) << 6);
                result.codepoint |= (cont_byte[2] & 0x3f);
                result.inc = 4;
            }
        }
    }
    }

    return result;
}

internal UnicodeDecode utf16_decode(U16* str, U64 max) {
    UnicodeDecode result;

    result.codepoint = str[0];
    result.inc = 1;

    if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000) {
        result.codepoint = ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
        result.inc = 2;
    }

    return result;
}

internal U32 utf8_encode(U8* str, U32 codepoint) {
    U32 inc = 0;

    if (codepoint <= 0x7F) {
        str[0] = (U8)codepoint;
        inc = 1;
    } else if (codepoint <= 0x7FF) {
        str[0] = (0x3 << 6) | ((codepoint >> 6) & 0x1f);
        str[1] = (1 << 7) | (codepoint & 0x3f);
        inc = 2;
    } else if (codepoint <= 0xFFFF) {
        str[0] = (0x7 << 5) | ((codepoint >> 12) & 0xf);
        str[1] = (1 << 7) | ((codepoint >> 6) & 0x3f);
        str[2] = (1 << 7) | (codepoint & 0x3f);
        inc = 3;
    } else if (codepoint <= 0x10FFFF) {
        str[0] = (0xf << 4) | ((codepoint >> 18) & 0x7);
        str[1] = (1 << 7) | ((codepoint >> 12) & 0x3f);
        str[2] = (1 << 7) | ((codepoint >> 6) & 0x3f);
        str[3] = (1 << 7) | (codepoint & 0x3f);
        inc = 4;
    } else {
        str[0] = '?';
        inc = 1;
    }

    return inc;
}

internal U32 utf16_encode(U16* str, U32 codepoint) {
    U32 inc = 1;

    if (codepoint == U32_MAX)
        str[0] = (U16)'?';
    else if (codepoint < 0x10000)
        str[0] = (U16)codepoint;
    else {
        U32 v = codepoint - 0x10000;
        str[0] = (U16)(0xD800 + (v >> 10));
        str[1] = (U16)(0xDC00 + (v & 0x3ff));
        inc = 2;
    }

    return inc;
}

internal U32 utf8_size(U32 cp) {
    if (cp <= 0x7F)
        return 1;
    if (cp <= 0x7FF)
        return 2;
    if (cp <= 0xFFFF)
        return 3;
    return 4;
}

internal U32 utf16_size(U32 cp) {
    if (cp > 0x10FFFF)
        return 0;
    if (cp >= 0xD800 && cp <= 0xDFFF)
        return 0;
    if (cp <= 0xFFFF)
        return 1;
    return 2;
}

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

    out = arena_push_array(arena, U8, size);

    for (ptr = str.data; ptr < opl;) {
        d = utf16_decode(ptr, opl - ptr);
        ptr += d.inc;
        at += utf8_encode(out + at, d.codepoint);
    }

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

    out = arena_push_array(arena, U8, size);

    for (ptr = str.data; ptr < opl; ptr++)
        at += utf8_encode(out + at, *ptr);

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

    result.data = arena_push_array(arena, U8, size);
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
        vsnprintf((char*)result.data, result.size, fmt, args);
    }

    va_end(args);
    return result;
}

internal U32 str8_match(Str8 a, Str8 b, Str8MatchFlag flags) {
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

internal U64 str8_find(Str8 target, Str8 query, U64 offset, Str8MatchFlag flags) {
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

internal U64 str8_find_reverse(Str8 target, Str8 query, U64 offset, Str8MatchFlag flags) {
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

    return target;
}

internal char *str8_copy_to_cstr(Arena *arena, Str8 str) {
    char* target;

    target = arena_push_array(arena, char, str.size);
    mem_copy_array(target, str.data, str.size);

    return target;
}

internal Str8 str8_slice(Str8 str, U64 begin, U64 end) {
    begin = clamp_top(begin, str.size);
    end = clamp_top(end, str.size);

    str.data += begin;
    str.size = (end - begin);

    return str;
}

internal Str8 str8_nt(Arena* arena, Str8 str) {
    Str8 new_str;

    new_str = str8_from_size(arena, str.size + 1);
    mem_copy(new_str.data, str.data, str.size);

    new_str.data[str.size] = 0;
    new_str.size--;

    return new_str;
}

internal U32 str8_is_integer(Str8 str) {
    U32 i, result;

    result = 1;

    for_each_index(i, str.size) {
        if (!char_is_numeric(str.data[i]))
            result = 0;
    }

    return result;
}

internal U64 u64_from_str8(Str8 str, U8 base) {
    U32 i;
    U64 number;

    number = 0;

    for_each_index(i, str.size) {
        number *= base;
        number += (CHAR_LUT[str.data[i]] & CHAR_DIGIT_MASK);
    }

    return number;
}

internal Str8 str8_from_u64(Arena* arena, U64 num) {
    Str8 str;
    U64 temp;

    temp = num;

    for (str.size = 0; temp; temp /= 10)
        str.size++;

    str = str8_from_size(arena, str.size);

    for (temp = str.size; temp > 0; temp--) {
        str.data[temp - 1] = (U8)('0' + (num % 10));
        num /= 10;
    }

    return str;
}

internal Str16 str16_from_cstr(U16* str) {
    U64 size;
    Str16 result;

    for (size = 0; str[size] != 0; size++);

    result.data = str;
    result.size = size;

    return result;
}

internal Str16 str16_from_str8(Arena* arena, Str8 str) {
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

        out = arena_push_array(arena, U16, size);

        ptr = str.data;

        for (at = 0; ptr < opl;) {
            d = utf8_decode(ptr, opl - ptr);
            ptr += d.inc;
            at += utf16_encode(out + at, d.codepoint);
        }

        result.data = out;
        result.size = size;
    }

    return result;
}

internal Str16 str16_from_size(Arena* arena, U64 size) {
    Str16 result;

    result.data = arena_push_array(arena, U16, size);
    result.size = size;

    return result;
}

internal Str16 str16_nt(Arena* arena, Str16 str) {
    Str16 new_str;

    new_str = str16_from_size(arena, str.size + 1);
    mem_copy(new_str.data, str.data, str.size * sizeof(U16));

    new_str.data[str.size] = 0;
    new_str.size--;

    return new_str;
}

internal Str16 str16_concat(Arena* arena, Str16 a, Str16 b) {
    Str16 result;

    result = str16_from_size(arena, a.size + b.size);

    mem_copy(result.data, a.data, a.size * sizeof(U16));
    mem_copy(result.data + a.size, b.data, b.size * sizeof(U16));

    return result;
}

internal Str16 str16_concat_n_(Arena* arena, ...) {
    va_list args;
    U16* buffer;
    U64 size;
    Str16 result;
    U64 offset;

    va_start(args, arena);

    for (size = 0; result.data; size += result.size) {
        result = va_arg(args, Str16);
    }

    va_end(args);

    buffer = arena_push_array(arena, U16, size);

    va_start(args, arena);

    for (offset = 0; result.data || result.size != STRNPOS; offset += result.size) {
        result = va_arg(args, Str16);
        mem_copy(buffer + offset, result.data, result.size * sizeof(U16));
    }

    va_end(args);

    result.size = size;
    result.data = buffer;

    return result;
}

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

        out = arena_push_array(arena, U32, size);

        ptr = str.data;

        for (at = 0; ptr < opl;) {
            d = utf8_decode(ptr, opl - ptr);
            ptr += d.inc;
            out[at++] = d.codepoint;
        }

        result.data = out;
        result.size = size;
    }

    return result;
}

internal Str32 str32_from_size(Arena* arena, U64 size) {
    Str32 result;

    result.data = arena_push_array(arena, U32, size);
    result.size = size;

    return result;
}

internal Str32 str32_nt(Arena* arena, Str32 str) {
    Str32 new_str;

    new_str = str32_from_size(arena, str.size + 1);
    mem_copy(new_str.data, str.data, str.size * sizeof(U32));

    new_str.data[str.size] = 0;
    new_str.size--;

    return new_str;
}

internal Str32 str32_concat_n_(Arena* arena, ...) {
    va_list args;
    U32* buffer;
    U64 size;
    Str32 result;
    U64 offset;

    va_start(args, arena);

    for (size = 0; result.data; size += result.size) {
        result = va_arg(args, Str32);
    }

    va_end(args);

    buffer = arena_push_array(arena, U32, size);

    va_start(args, arena);

    for (offset = 0; result.data || result.size != STRNPOS; offset += result.size) {
        result = va_arg(args, Str32);
        mem_copy(buffer + offset, result.data, result.size * sizeof(U32));
    }

    va_end(args);

    result.size = size;
    result.data = buffer;

    return result;
}

internal Str32 str32_concat(Arena* arena, Str32 a, Str32 b) {
    Str32 result;

    result = str32_from_size(arena, a.size + b.size);

    mem_copy(result.data, a.data, a.size * sizeof(U32));
    mem_copy(result.data + a.size, b.data, b.size * sizeof(U32));

    return result;
}