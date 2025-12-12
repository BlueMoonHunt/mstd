#include "mstd.h"
#include <string.h>

typedef struct Arena Arena;
struct Arena {
    u8* base;
    u64 commited;
    u64 commit_size;
    u64 cursor;
    enum_class(ArenaFlag, u32) flags;
    u64 reserved;
};

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp {
    Arena* arena;
    u64 cursor;
};

b32 mem_is_pow2(void* memory) {
    return is_pow2((uaddress)memory);
}

b32 mem_is_aligned(void* memory, u64 alignment) {
    return align_up_pad_pow2((uaddress)memory, alignment) == 0;
}

b32 mem_compare(void* memory1, void* memory2, u64 count) {
    return memcmp(memory1, memory2, count) == 0;
}

void mem_copy(void* destination, void* source, u64 size) {
    memcpy(destination, source, size);
}

void mem_set(void* memory, u8 value, u64 size) {
    memset(memory, value, size);
}

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

str8 _str8_create(u8* str, u64 size) {
    str8 result = { .str = str, .size = size };
    return result;
}

str16 _str16_create(u16* str, u64 size) {
    str16 result = { .str = str, .size = size };
    return result;
}

str8 str8_of_size(u64 size, Arena* arena) {
    assert(arena);
    str8 str = {.size = size};
    str.str = arena_push_array(arena, u8, size + 1);
    str.str[str.size] = 0;
    return str;
}

str16 str16_of_size(u64 size, Arena* arena) {
    assert(arena);
    str16 str = { .size = size };
    str.str = arena_push_array(arena, u16, str.size + 1);
    str.str[str.size] = 0;
    return str;
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
    str8 str_new = str8_of_size(str.size, arena);
    mem_copy(str_new.str, str.str, str.size);
    str.str[str.size] = 0;
    return str_new;
}

str16 str16_copy(Arena* arena, const str16 str) {
    assert(arena && str.str);
    str16 str_new = str16_of_size(str.size, arena);
    mem_copy(str_new.str, str.str, str.size);
    str_new.str[str_new.size] = 0;
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

Arena* arena_alloc(const ArenaCreateInfo create_info) {
    u64 reserve_size = create_info.reserve_size;
    u64 commit_size = create_info.commit_size;

    if(create_info.allocate_large_pages) {
        reserve_size = align_up_pow2(reserve_size, os_get_large_page_size());
        commit_size = align_up_pow2(commit_size, os_get_large_page_size());
    }
    else {
        reserve_size = align_up_pow2(reserve_size, os_get_page_size());
        commit_size = align_up_pow2(commit_size, os_get_page_size());
    }

    void* base;
    if(create_info.allocate_large_pages) {
        base = os_reserve_large(reserve_size);
        os_commit_large(base, commit_size);
    }
    else {
        base = os_reserve(reserve_size);
        os_commit(base, commit_size);
    }

    Arena* arena = (Arena*)base;
    arena->base = base;
    arena->commit_size = commit_size;
    arena->commited = commit_size;
    arena->cursor = 0;
    arena->flags = (create_info.allocate_large_pages) ? arena_flag_allocate_large_pages : arena_flag_none;
    arena->reserved = reserve_size;

    return arena;
}

void arena_release(Arena* arena) {
    os_release(arena, arena->reserved);
}

void* arena_push(Arena* arena, const u64 size, const u64 alignment) {
    assert(arena);
    u64 begin_pos = align_up_pow2(arena->cursor, alignment);
    u64 end_pos = begin_pos + size;

    if (arena->commited < end_pos) {
        u64 commit_size = min(align_up_pow2(end_pos, arena->commit_size), arena->reserved) - arena->commited;

        void* commit_ptr = (void*)(arena->base + arena->commited);
        b32 commit_success = 0;

        if (arena->flags & arena_flag_allocate_large_pages)
            commit_size = os_commit_large(commit_ptr, commit_size);
        else
            commit_size = os_commit(commit_ptr, commit_size);

        if (!commit_success)
            arena->commited += commit_size;
        }

    void* result = 0;
    if(arena->commited >= end_pos){
        result = (void*)(arena->base + begin_pos);
        arena->cursor = end_pos;
    }

    return result;
}

void arena_clear(Arena* arena) {
    arena->cursor = 0;
}

ArenaTemp* arena_temp_begin(Arena* arena) {
    assert(arena);
    const u64 cursor = arena->cursor;

    ArenaTemp* temp = (ArenaTemp*)arena_push(arena, sizeof(ArenaTemp), alignof(ArenaTemp));
    temp->arena = arena;
    temp->cursor = cursor;

    return temp;
}

void arena_temp_end(ArenaTemp* arena_temp) {
    assert(arena_temp);
    arena_temp->arena->cursor = arena_temp->cursor;
}

void random_seed_pcg(RandomNumberGenerator* rng, const u64 state, const u64 stream) {
    rng->state.low = 0;
    rng->state.high = 0;

    rng->inc.low = (stream << 1) | 1;
    rng->inc.high = state;

    random_update_pcg_state(rng);
    rng->state.low += stream;
    random_update_pcg_state(rng);
}

u64 random_pcg(RandomNumberGenerator* rng) {
    const u128 oldstate = rng->state;
    random_update_pcg_state(rng);
    unsigned int rot = (unsigned int)(oldstate.high >> 58);
    u64 xorshifted = (oldstate.high ^ oldstate.low) >> 59;
    return rotate_right_u64(xorshifted, rot);
}

#if defined(OS_WINDOWS)
    #include "ext/mstd_win32.c"
#elif defined(OS_LINUX)
#elif defined(OS_MAC)
#endif