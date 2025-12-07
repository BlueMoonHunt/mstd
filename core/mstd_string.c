#include "mstd_core.h"

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

string8 _str8(u8* str, u64 size) {
    string8 result = {.str = str, .size = size};
    return result;
}

string16 _str16(u16* str, u64 size) {
    string16 result = {.str = str, .size = size};
    return result;
}

string8 str8_from_cstr(char* str) {
    string8 result = {.str = (u8*)str, .size = cstr8_length((u8*)str)};
    return result;
}

string16 str16_from_cstr(u16* str) {
    string16 result = {.str = (u16*)str, .size = cstr16_length((u16*)str)};
    return result;
}