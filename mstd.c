#if OS_WINDOWS
#include "mstd_win32.c"
#endif

////////////////////////////////
// Base Types

#if COMPILER_MSVC

internal force_inline u8 u32_count_zerol(u32 x) {
    unsigned long zeros = 0;
    return _BitScanReverse(&zeros, x) ? (u8)(31 - zeros) : 32;
}
internal force_inline u8 u64_count_zerol(u64 x) {
    unsigned long zeros = 0;
    return _BitScanReverse64(&zeros, x) ? (u8)(63 - zeros) : 64;
}

internal force_inline u8 u32_count_zeror(u32 x) {
    unsigned long zeros = 0;
    _BitScanForward(&zeros, x);
    return (u8)zeros;
}
internal force_inline u8 u64_count_zeror(u64 x) {
    unsigned long zeros = 0;
    _BitScanForward64(&zeros, x);
    return (u8)zeros;
}

internal force_inline i8 u32_msb(u32 x) {
    unsigned long where;
    return _BitScanReverse(&where, x) ? (i8)where : -1;
}
internal force_inline i8 u64_msb(u64 x) {
    unsigned long where;
    return _BitScanReverse64(&where, x) ? (i8)where : -1;
}

internal force_inline i8 u32_lsb(u32 x) {
    unsigned long where;
    return _BitScanForward(&where, x) ? (i8)where : -1;
}
internal force_inline i8 u64_lsb(u64 x) {
    unsigned long where;
    return _BitScanForward64(&where, x) ? (i8)where : -1;
}

#endif

////////////////////////////////
// Math

DEFINE_QUANT(i8)
DEFINE_QUANT(i16)
DEFINE_QUANT(i32)
DEFINE_QUANT(u8)
DEFINE_QUANT(u16)
DEFINE_QUANT(u32)

////////////////////////////////
// Arena

internal Arena *arena_alloc_opt(u64 reserve_size, char *file, u32 line, ArenaOpt opt) {
    u64 page_size = (opt.large_pages) ? mem_large_page_size() : mem_page_size();
    debug_assert(page_size);

    u64 actual_reserve = align_up_pow2(reserve_size, page_size);
    u64 initial_commit = align_up_pow2(ARENA_HEADER_SIZE, page_size);

    void *memory = mem_reserve(actual_reserve, opt.large_pages);
    debug_assert(memory);
    mem_commit(memory, initial_commit, opt.large_pages);

    Arena *arena = (Arena *)memory;
    arena->committed = initial_commit;
    arena->reserved = actual_reserve;
    arena->page_size = page_size;
    arena->cursor = ARENA_HEADER_SIZE;
    arena->can_commit_large_pages = opt.large_pages;
    arena->temp_stack_head = 0;
    arena->temp_stack_tail = 0;

#if MSTD_DEBUG
    arena->code_line_of_alloc = line;
    arena->code_file_of_alloc = file;
#else
    (void)(line);
    (void)(file);
#endif

    return arena;
}

internal void arena_release(Arena *arena) {
    debug_assert(arena);
    mem_release(arena, arena->reserved);
}

internal void *arena_push(Arena *arena, u64 size, u64 align) {
    debug_assert(arena);
    u64 begin_pos = align_up_pow2(arena->cursor, align);
    u64 end_pos = begin_pos + size;

    debug_assert(end_pos <= arena->reserved);

    if (end_pos > arena->committed) {
        u64 commit_target = align_up_pow2(end_pos, (u64)arena->page_size);
        u64 commit_size = clamp_top(commit_target, arena->reserved) - arena->committed;
        mem_commit((u8 *)arena + arena->committed, commit_size, arena->can_commit_large_pages);
        arena->committed = commit_target;
    }

    void *user_ptr = (u8 *)arena + begin_pos;
    arena->cursor = end_pos;

    return user_ptr;
}

internal void arena_reset(Arena *arena) {
    debug_assert(arena);

    if (arena->temp_stack_head)
        arena->cursor = (u8 *)arena->temp_stack_head - (u8 *)arena + sizeof(ArenaTempNode);
    else
        arena->cursor = ARENA_HEADER_SIZE;
}

internal void arena_temp_push(Arena *arena) {
    debug_assert(arena);
    ArenaTempNode *node = arena_push_struct(arena, ArenaTempNode);
    sll_stack_push(arena->temp_stack_head, arena->temp_stack_tail, node);
}

internal void arena_temp_pop(Arena *arena) {
    debug_assert(arena);

    if (arena->temp_stack_head) {
        u64 cursor = (u64)((u8 *)arena->temp_stack_head - (u8 *)arena);
        sll_stack_pop(arena->temp_stack_head, arena->temp_stack_tail);
        arena->cursor = cursor;
    }
}

internal void arena_temp_pop_all(Arena *arena) {
    debug_assert(arena);

    if (arena->temp_stack_tail) {
        arena->cursor = (u8 *)arena->temp_stack_tail - (u8 *)arena;
        arena->temp_stack_head = 0;
        arena->temp_stack_tail = 0;
    }
}

#define ARENA_SCRATCH_POOL_COUNT 4

global thread_var Arena *arena_scratch_pool[ARENA_SCRATCH_POOL_COUNT] = {0};
global thread_var u8 arena_scratch_pool_available_mask = (1u << ARENA_SCRATCH_POOL_COUNT) - 1;

internal Arena *arena_scratch_alloc(void) {
    i32 index = u32_lsb((u32)arena_scratch_pool_available_mask);

    if (index >= 0 && index < ARENA_SCRATCH_POOL_COUNT) {
        Arena *arena = arena_scratch_pool[index];
        if (arena == 0) {
            arena = arena_alloc(ARENA_DEFAULT_RESERVE_SIZE);
            arena_scratch_pool[index] = arena;
        }
        arena_reset(arena);
        arena_scratch_pool_available_mask &= ~(1u << index);
        return arena;
    }
    return 0;
}

internal void arena_scratch_release(Arena *arena) {
    debug_assert(arena);

    for (i32 i = 0; i < ARENA_SCRATCH_POOL_COUNT; ++i) {
        if (arena_scratch_pool[i] == arena) {
            arena_scratch_pool_available_mask |= (1u << i);
            return;
        }
    }
    trap();
}

////////////////////////////////
// Strings

internal Str8 str8_from_cstr(u8 *str) {
    u64 size = 0;
    for (; str[size] != 0; size++)
        ;

    Str8 result = {0};
    result.data = str;
    result.size = size;

    return result;
}

internal Str8 str8_from_mem_size(Arena *arena, u64 size) {
    debug_assert(arena);

    Str8 result = {0};

    result.data = arena_push_array(arena, u8, size + 1);
    result.data[size] = 0;
    result.size = size;

    return result;
}

internal Str8 str8_from_fmt(Arena *arena, char *fmt, ...) {
    Str8 result = {0};

    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    int length = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (length) {
        result = str8_from_mem_size(arena, length);
        vsnprintf((char *)result.data, result.size + 1, fmt, args);
        result.data[result.size] = 0;
    }

    va_end(args);
    return result;
}

internal Str8 str8_concat_args_till_str_npos(Arena *arena, ...) {
    va_list args;
    u64 size = 0;

    va_start(args, arena);
    for (;;) {
        Str8 s = va_arg(args, Str8);
        if (s.data == 0)
            break;
        size += s.size;
    }
    va_end(args);

    u8 *buffer = arena_push_array(arena, u8, size);
    u64 offset = 0;

    va_start(args, arena);
    for (;;) {
        Str8 s = va_arg(args, Str8);
        if (s.data == 0 && s.size == NPOS)
            break;
        mem_copy(buffer + offset, s.data, s.size);
        offset += s.size;
    }
    va_end(args);

    return (Str8){.size = size, .data = buffer};
}

internal Str8 str8_concat(Arena *arena, Str8 a, Str8 b) {
    debug_assert(arena);
    Str8 result = str8_from_mem_size(arena, a.size + b.size);

    mem_copy(result.data, a.data, a.size);
    mem_copy(result.data + a.size, b.data, b.size);
    result.data[result.size] = 0;

    return result;
}

internal u32 str8_match_opt(Str8 a, Str8 b, Str8MatchOpt opt) {
    u32 match = 0;

    if (a.size == b.size && (opt.case_insensitive || opt.slash_insensitive)) {
        for (u64 i = 0; i < a.size; ++i) {
            u8 ca = a.data[i];
            u8 cb = b.data[i];
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

internal u64 str8_find_opt(Str8 string, Str8 substring, u64 offset, Str8MatchOpt opt) {
    u64 result = NPOS;

    if (substring.size > 0 && string.size >= (substring.size + offset)) {
        u8 *stop = string.data + string.size - substring.size + 1;

        for (u8 *cursor = string.data + offset; cursor < stop; cursor++) {
            u8 c_substr = substring.data[0];
            u8 c_str = *cursor;

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
            if (c_str == c_substr && str8_match_opt((Str8){.data = cursor, .size = substring.size}, substring, opt)) {
                result = (u64)(cursor - string.data);
                break;
            }
        }
    }

    return result;
}

internal u64 str8_find_reverse_opt(Str8 string, Str8 substring, u64 offset, Str8MatchOpt opt) {
    u64 result = NPOS;

    if (substring.size > 0 && string.size >= (substring.size + offset))
        for (u8 *cursor = string.data + string.size - offset - substring.size; cursor >= string.data; cursor--) {
            u8 c_substr = substring.data[0];
            u8 c_str = *cursor;

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
            if (c_str == c_substr && str8_match_opt((Str8){.data = cursor, .size = substring.size}, substring, opt)) {
                result = (u64)(cursor - string.data);
                break;
            }
        }

    return result;
}

internal Str8View str8_slice_opt(Str8View str, u64 pos, Str8ViewOpt opt) {
    pos = clamp_top(pos, str.size);

    if (opt.postfix) {
        str.data += pos;
        str.size -= pos;
    } else
        str.size = pos;

    if (opt.delimiter.size > 0) {
        for (u64 i = 0; i < str.size; i++) {
            for (u64 j = 0; j < opt.delimiter.size; j++) {
                if (opt.delimiter.data[j] == str.data[i]) {
                    str.size = i;
                    goto done;
                }
            }
        }
    }

done:
    return str;
}

internal Str8 str8_copy(Arena *arena, Str8 str) {
    debug_assert(arena);
    Str8 string = str8_from_mem_size(arena, str.size);

    mem_copy_array(string.data, str.data, str.size);
    string.data[string.size] = 0;

    return string;
}

internal char *str8_copy_to_cstr(Arena *arena, Str8 str) {
    debug_assert(arena);
    char *string = arena_push_array(arena, char, str.size + 1);
    mem_copy_array(string, str.data, str.size);
    string[str.size] = 0;
    return string;
}

internal Str16 str16_from_cstr(u16 *str) {
    u64 size = 0;
    for (; str[size] != 0; size++)
        ;

    Str16 result = {0};
    result.data = str;
    result.size = size;

    return result;
}

internal Str16 str16_from_mem_size(Arena *arena, u64 size) {
    debug_assert(arena);
    Str16 result = {0};

    result.data = arena_push_array(arena, u16, size + 1);
    result.data[size] = 0;
    result.size = size;

    return result;
}

const global u8 utf8_class[32] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 5,
};

internal UnicodeDecode utf8_decode(u8 *str, u64 max) {
    UnicodeDecode result = {1, u32_max};
    u8 byte = str[0];
    u8 byte_class = utf8_class[byte >> 3];
    switch (byte_class) {
    case 1: {
        result.codepoint = byte;
    } break;
    case 2: {
        if (1 < max) {
            u8 cont_byte = str[1];
            if (utf8_class[cont_byte >> 3] == 0) {
                result.codepoint = (byte & 0x1f) << 6;
                result.codepoint |= (cont_byte & 0x3f);
                result.inc = 2;
            }
        }
    } break;
    case 3: {
        if (2 < max) {
            u8 cont_byte[2] = {str[1], str[2]};
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
            u8 cont_byte[3] = {str[1], str[2], str[3]};
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

internal UnicodeDecode utf16_decode(u16 *str, u64 max) {
    UnicodeDecode result = {1, u32_max};
    result.codepoint = str[0];
    result.inc = 1;
    if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000) {
        result.codepoint = ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
        result.inc = 2;
    }
    return result;
}

internal u32 utf8_encode(u8 *str, u32 codepoint) {
    u32 inc = 0;
    if (codepoint <= 0x7F) {
        str[0] = (u8)codepoint;
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

internal u32 utf16_encode(u16 *str, u32 codepoint) {
    u32 inc = 1;
    if (codepoint == u32_max)
        str[0] = (u16)'?';
    else if (codepoint < 0x10000)
        str[0] = (u16)codepoint;
    else {
        u32 v = codepoint - 0x10000;
        str[0] = (u16)(0xD800 + (v >> 10));
        str[1] = (u16)(0xDC00 + (v & 0x3ff));
        inc = 2;
    }
    return inc;
}

internal u32 utf8_size(u32 cp) {
    if (cp <= 0x7F)
        return 1;
    if (cp <= 0x7FF)
        return 2;
    if (cp <= 0xFFFF)
        return 3;
    return 4;
}

internal u32 utf16_size(u32 cp) {
    if (cp > 0x10FFFF)
        return 0;
    if (cp >= 0xD800 && cp <= 0xDFFF)
        return 0;
    if (cp <= 0xFFFF)
        return 1;
    return 2;
}

internal Str8 str8_from_16(Arena *arena, Str16 str) {
    debug_assert(arena);
    Str8 result = {0};

    u16 *ptr = str.data;
    u16 *opl = ptr + str.size;
    u64 size = 0;

    for (; ptr < opl;) {
        UnicodeDecode d = utf16_decode(ptr, opl - ptr);
        ptr += d.inc;
        size += utf8_size(d.codepoint);
    }

    u8 *out = arena_push_array(arena, u8, size + 1);

    ptr = str.data;
    u64 at = 0;
    for (; ptr < opl;) {
        UnicodeDecode d = utf16_decode(ptr, opl - ptr);
        ptr += d.inc;
        at += utf8_encode(out + at, d.codepoint);
    }

    out[size] = 0;
    result.data = out;
    result.size = size;
    return result;
}

internal Str16 str16_from_8(Arena *arena, Str8 str) {
    debug_assert(arena);
    Str16 result = {0};
    if (!str.size)
        return result;

    u8 *ptr = str.data;
    u8 *opl = ptr + str.size;

    u64 size = 0;
    for (; ptr < opl;) {
        UnicodeDecode d = utf8_decode(ptr, opl - ptr);
        ptr += d.inc;
        size += utf16_size(d.codepoint);
    }

    u16 *out = arena_push_array(arena, u16, size + 1);

    ptr = str.data;
    u64 at = 0;
    for (; ptr < opl;) {
        UnicodeDecode d = utf8_decode(ptr, opl - ptr);
        ptr += d.inc;
        at += utf16_encode(out + at, d.codepoint);
    }

    out[size] = 0;

    result.data = out;
    result.size = size;
    return result;
}

internal Str8 str8_from_32(Arena *arena, Str32 str) {
    debug_assert(arena);
    Str8 result = {0};
    if (!str.size)
        return result;

    u32 *ptr = str.data;
    u32 *opl = ptr + str.size;

    u64 size = 0;
    for (; ptr < opl; ptr++) {
        size += utf8_size(*ptr);
    }

    u8 *out = arena_push_array(arena, u8, size + 1);

    u64 at = 0;
    for (ptr = str.data; ptr < opl; ptr++) {
        at += utf8_encode(out + at, *ptr);
    }

    out[size] = 0;
    result.data = out;
    result.size = size;
    return result;
}

internal Str32 str32_from_8(Arena *arena, Str8 str) {
    debug_assert(arena);
    Str32 result = {0};
    if (!str.size)
        return result;

    u8 *ptr = str.data;
    u8 *opl = ptr + str.size;

    u64 size = 0;
    for (; ptr < opl;) {
        UnicodeDecode d = utf8_decode(ptr, opl - ptr);
        ptr += d.inc;
        size += 1;
    }

    u32 *out = arena_push_array(arena, u32, size + 1);

    ptr = str.data;
    u64 at = 0;
    for (; ptr < opl;) {
        UnicodeDecode d = utf8_decode(ptr, opl - ptr);
        ptr += d.inc;
        out[at++] = d.codepoint;
    }

    out[size] = 0;
    result.data = out;
    result.size = size;
    return result;
}

////////////////////////////////
// Module: Timer

internal Timer timer_start(void) {
    Timer timer = {0};
    timer.ticks = clock_ticks_now();
    timer.delta = 16666.6f;

    timer.resolution_us = clock_resolution_us();
    timer.inverse_ticks_per_us = 1000000.0 / (f64)timer.resolution_us;

    return timer;
}

internal void timer_update(Timer *timer) {
    u64 current_ticks = clock_ticks_now();

    u64 elapsed_ticks = (current_ticks > timer->ticks) ? (current_ticks - timer->ticks) : 0;
    double us = (double)elapsed_ticks * timer->inverse_ticks_per_us;
    if (us > 200000.0)
        us = 200000.0;

    timer->delta = (float)us;
    timer->ticks = current_ticks;
}

internal u64 timer_get_timestamp(Timer *timer) { return (u64)(timer->ticks * timer->inverse_ticks_per_us); }

//////////////////////////////
// DS: DArray

internal force_inline void *darray_handle(Arena *arena, DArrayHeader *header, DArrayMetaData meta, u64 index) {
    u8 **chunks = (u8 **)(header + 1);

    u64 i_shift = index >> meta.shift;
    i8 chunks_i = u64_msb(i_shift + 1);

    if (arena && chunks_i < meta.chunks_max) {
        for (i8 i = 0; i <= chunks_i; ++i) {
            if (chunks[i] == 0) {
                u64 tier_size = 1ULL << clamp_top((i + meta.shift), 63);
                chunks[i] = (u8 *)arena_push(arena, tier_size * meta.el_size, 8);
            }
        }
    }

    u64 elem_i = index;

    if (chunks_i > 0) {
        u64 base = (1ULL << (chunks_i + meta.shift)) - (1ULL << meta.shift);
        elem_i -= base;
    }

    debug_assert(chunks[chunks_i]);
    return chunks[chunks_i] + (elem_i * meta.el_size);
}
