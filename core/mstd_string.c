#include "mstd_core.h"
#include <arena/mstd_arena.h>

b32 char_is_space(u8 c) {
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

b32 char_is_upper(u8 c) {
    return ('A' <= c && c <= 'Z');
}

b32 char_is_lower(u8 c) {
    return ('a' <= c && c <= 'z');
}

b32 char_is_alphabet(u8 c) {
    return char_is_lower(c) || char_is_upper(c);
}

b32 char_is_digit(u8 c, u32 base) {
    u8 val = 0xFF;

    if (c >= '0' && c <= '9')      val = c - '0';
    else if (c >= 'a' && c <= 'f') val = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') val = c - 'A' + 10;

    return (val < base);
}

u8 char_to_lower(u8 c) {
    return (char_is_upper(c) ? c + ('a' - 'A') : c);
}

u8 char_to_upper(u8 c) {
    return (char_is_lower(c) ? c + ('A' - 'a') : c);
}

u64 cstr8_length(u8* c) {
    u8* p = c;
    for (;*p != 0; p++);
    return (p - c);
}

u64 cstr16_length(u16* c) {
    u16* p = c;
    for (;*p != 0; p++);
    return (p - c);
}

str8 _str8(u8* str, u64 size) {
    str8 result = { .str = str, .size = size };
    return result;
}

str16 _str16(u16* str, u64 size) {
    str16 result = { .str = str, .size = size };
    return result;
}

str8 str8_from_cstr(const char* str) {
    str8 result = { .str = (u8*)str, .size = cstr8_length((u8*)str) };
    return result;
}

str16 str16_from_cstr(const u16* str) {
    str16 result = { .str = (u16*)str, .size = cstr16_length((u16*)str) };
    return result;
}

str8 str8_copy(Arena* arena, const str8 str) {
    assert(arena && str.str);
    str8 str_new = { .size = str.size };
    str_new.str = arena_push_array(arena, u8, str.size + 1);
    mem_copy(str_new.str, str.str, str.size);
    str_new.str[str.size] = 0;
    return str_new;
}

str16 str16_copy(Arena* arena, const str16 str) {
    assert(arena && str.str);
    str16 str_new = { .size = str.size };
    str_new.str = arena_push_array(arena, u16, str.size + 1);
    mem_copy(str_new.str, str.str, str.size);
    str_new.str[str.size] = 0;
    return str_new;
}

str8 str8_to_lower(Arena* arena, const str8 str) {
    str8 result = str8_copy(arena, str);
    for (u64 i = 0; i < result.size; i++)
        result.str[i] = char_to_lower(result.str[i]);
    return result;
}

str8 str8_to_upper(Arena* arena, const str8 str) {
    str8 result = str8_copy(arena, str);
    for (u64 i = 0; i < result.size; i++)
        result.str[i] = char_to_upper(result.str[i]);
    return result;
}

str8 str8_concat(Arena* arena, const str8 a, const str8 b) {
    str8 result = { .size = a.size + b.size };
    result.str = arena_push_array(arena, u8, result.size + 1);
    mem_copy(result.str, a.str, b.size);
    mem_copy(result.str + a.size, b.str, b.size);
    result.str[result.size] = 0;
    return result;
}

b32 str8_equal(const str8 a, const str8 b) {
    return (a.size == b.size) ? mem_compare(a.str, b.str, a.size) : 0;
}
