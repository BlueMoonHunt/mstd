#ifndef MSTD_H
#define MSTD_H

// ----------------------------------------------------------------------------
// INTERFACE
// ----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#define COMPILER_MSVC 1
#elif defined(__clang__)
#define COMPILER_CLANG 1
#elif defined(__GNUC__)
#define COMPILER_GCC 1
#else
#warning "Unknown Compiler"
#endif

#if defined(_WIN32)
#define OS_WINDOWS 1
#elif defined(__linux__)
#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#warning "Unknown Operating System"
#endif

#if defined(_M_AMD64) || defined(__amd64__) || defined(__x86_64__)
#define ARCH_X64 1
#elif defined(_M_IX86) || defined(__i386__)
#define ARCH_X86 1
#elif defined(_M_ARM) || defined(__arm__) || defined(__aarch64__)
#define ARCH_ARM 1
#else
#warning "Unknown Architecture"
#endif

#define internal static
#define global static
#define local_persist static

////////////////////////////////
// TYPES

#include <stdint.h>
#include <stdalign.h>

#ifdef USE_CUSTOM_ASSERT
#define mstd_assert USE_CUSTOM_ASSERT
#else
#include <assert.h>
#define mstd_assert assert
#endif


typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t uaddress;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef intptr_t iaddress;

typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

#define true 1
#define false 0

typedef float f32;
typedef double f64;

#define enum_class(type, size_type) size_type

#define UNUSED(value) (void)(value)

#define align_up_pow2(value,pow_of_2)   (((value) + (pow_of_2) - 1)&(~((pow_of_2) - 1)))
#define align_down_pow2(value,pow_of_2) ((value)&(~((pow_of_2) - 1)))
#define align_up_pad_pow2(value,pow_of_2)  ((0-(value)) & ((pow_of_2) - 1))

#define is_pow2(value)          ((value)!=0 && ((value)&((value)-1))==0)
#define is_pow2_or_0(value)    ((((value) - 1)&(value)) == 0)

#define get_min(a, b) ((a) < (b)) ? a : b
#define get_max(a, b) ((a) > (b)) ? a : b
#define clamp(low, high, value) get_min(get_max(low, value), high)

#define KB(value) ((value) << 10)
#define MB(value) ((value) << 20)
#define GB(value) ((value) << 30)
#define TB(value) ((value) << 40)

////////////////////////////////
// MEMORY

b8 mem_is_pow2(void* memory);
b8 mem_is_aligned(void* memory, u64 alignment);
b8 mem_compare(void* data1, void* data2, u64 count);
void mem_copy(void* destination, void* source, u64 size);
void mem_set(void* data, u8 value, u64 size);
#define mem_zero(data, size) mem_set(data, 0, size)

typedef struct ArenaCreateInfo ArenaCreateInfo;
struct ArenaCreateInfo {
    u64 reserve_size;
    u64 commit_size;
    b8 allocate_large_pages;
};

typedef struct Arena Arena;
typedef struct ArenaTemp ArenaTemp;

Arena* arena_alloc(const ArenaCreateInfo create_info);
void arena_release(Arena* arena);
void* arena_push(Arena* arena, const u64 size, const u64 align);
void arena_clear(Arena* arena);

ArenaTemp* arena_temp_begin(Arena* arena);
void arena_temp_end(ArenaTemp* arena_temp);

#define arena_scratch(arena) for (ArenaTemp *_temp_ = arena_temp_begin(arena); _temp_ != NULL; arena_temp_end(_temp_), _temp_ = NULL)
#define arena_push_array(arena, Type, count) (Type*)arena_push((arena), sizeof(Type)*(count), alignof(Type))
#define arena_push_struct(arena, Type) (Type*)arena_push((arena), sizeof(Type), alignof(Type))

////////////////////////////////
// OS

typedef struct SystemInfo SystemInfo;
struct SystemInfo {
    u64 page_size;
    u64 large_page_size;
    u64 allocation_granularity;
    u32 processors;
    u32 numa_nodes;
    u64 cache_line_size;
};

typedef struct ProcessInfo ProcessInfo;
struct ProcessInfo {
    u32 pid;
    u32 group_id;
    u64 affinity_mask;
    u64 page_file_limit;
};

SystemInfo* os_get_system_info(Arena* arena);
ProcessInfo* os_get_process_info(Arena* arena);

u64 os_get_page_size();
u64 os_get_large_page_size();
void* os_reserve(u64 size);
void* os_reserve_large(u64 size);
b8 os_commit(void* ptr, u64 size);
b8 os_commit_large(void* ptr, u64 size);
void os_decommit(void* ptr, u64 size);
void os_release(void* ptr, u64 size);

////////////////////////////////
// RANDOM

////////////////////////////////
// UTF8 STRING

typedef struct str8 str8;
struct str8 {
    u8* str;
    u64 size;
};

b8 char_is_space(u8 c);
b8 char_is_upper(u8 c);
b8 char_is_lower(u8 c);
b8 char_is_alphabet(u8 c);
b8 char_is_digit(u8 c, u32 base);
u8 char_to_lower(u8 c);
u8 char_to_upper(u8 c);

u64 cstr8_length(u8* c);

b8 str8_equal(const str8 a, const str8 b);
str8 str8_from_cstr(const u8* str);
#define str8_literal(literal) str8_from_cstr((u8*)literal)
str8 str8_of_size(u64 size, Arena* arena);
str8 str8_copy(const str8 str, Arena* arena);
str8 str8_to_lower(const str8 str, Arena* arena);
str8 str8_to_upper(const str8 str, Arena* arena);
str8 str8_concat(const str8 a, const str8 b, Arena* arena);

////////////////////////////////
// UTF16 STRING

typedef struct str16 str16;
struct str16 {
    u16* str;
    u64 size;
};

u64 cstr16_length(u16* c);

str16 str16_of_size(u64 size, Arena* arena);
str16 str16_from_cstr(const u16* str);
#define str16_literal(literal) str16_from_cstr((u16*)literal)
str16 str16_copy(const str16 str, Arena* arena);
str16 str16_from_str8(str8 str, Arena* arena);

// ----------------------------------------------------------------------------
// IMPLEMENTATION
// ----------------------------------------------------------------------------
#if defined(MSTD_IMPLEMENTATION)

#include <string.h>

typedef struct Arena Arena;
struct Arena {
    u8* base;
    u64 commited;
    u64 commit_size;
    u64 cursor;
    b8 has_large_pages;
    u64 reserved;
};

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp {
    Arena* arena;
    u64 cursor;
};

b8 mem_is_pow2(void* memory) {
    return is_pow2((uaddress)memory);
}
b8 mem_is_aligned(void* memory, u64 alignment) {
    return align_up_pad_pow2((uaddress)memory, alignment) == 0;
}
b8 mem_compare(void* memory1, void* memory2, u64 count) {
    return memcmp(memory1, memory2, count) == 0;
}
void mem_copy(void* destination, void* source, u64 size) {
    memcpy(destination, source, size);
}
void mem_set(void* memory, u8 value, u64 size) {
    memset(memory, value, size);
}

Arena* arena_alloc(const ArenaCreateInfo create_info) {
    u64 reserve_size = create_info.reserve_size;
    u64 commit_size = create_info.commit_size;

    if (create_info.allocate_large_pages) {
        reserve_size = align_up_pow2(reserve_size, os_get_large_page_size());
        commit_size = align_up_pow2(commit_size, os_get_large_page_size());
    }
    else {
        reserve_size = align_up_pow2(reserve_size, os_get_page_size());
        commit_size = align_up_pow2(commit_size, os_get_page_size());
    }

    void* base;
    if (create_info.allocate_large_pages) {
        base = os_reserve_large(reserve_size);
        mstd_assert(base);
        os_commit_large(base, commit_size);
    }
    else {
        base = os_reserve(reserve_size);
        mstd_assert(base);
        os_commit(base, commit_size);
    }

    Arena* arena = (Arena*)base;
    arena->base = (u8*)base;
    arena->commit_size = commit_size;
    arena->commited = commit_size;
    arena->cursor = 0;
    arena->has_large_pages = (create_info.allocate_large_pages) ? true : false;
    arena->reserved = reserve_size;

    return arena;
}

void arena_release(Arena* arena) {
    mstd_assert(arena);
    os_release(arena, arena->reserved);
}

void* arena_push(Arena* arena, const u64 size, const u64 alignment) {
    mstd_assert(arena);
    u64 begin_pos = align_up_pow2(arena->cursor, alignment);
    u64 end_pos = begin_pos + size;

    if (arena->commited < end_pos) {
        u64 commit_size = get_min(align_up_pow2(end_pos, arena->commit_size), arena->reserved) - arena->commited;

        void* commit_ptr = (void*)(arena->base + arena->commited);
        b8 commit_success = 0;

        if (arena->has_large_pages)
            commit_success = os_commit_large(commit_ptr, commit_size);
        else
            commit_success = os_commit(commit_ptr, commit_size);

        if (commit_success)
            arena->commited += commit_size;
    }

    void* result = 0;
    if (arena->commited >= end_pos) {
        result = (void*)(arena->base + begin_pos);
        arena->cursor = end_pos;
    }

    return result;
}

void arena_clear(Arena* arena) {
    mstd_assert(arena);
    arena->cursor = 0;
}

ArenaTemp* arena_temp_begin(Arena* arena) {
    mstd_assert(arena);
    const u64 cursor = arena->cursor;

    ArenaTemp* temp = (ArenaTemp*)arena_push(arena, sizeof(ArenaTemp), alignof(ArenaTemp));
    temp->arena = arena;
    temp->cursor = cursor;

    return temp;
}

void arena_temp_end(ArenaTemp* arena_temp) {
    mstd_assert(arena_temp);
    arena_temp->arena->cursor = arena_temp->cursor;
}

b8 char_is_space(u8 c) {
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

b8 char_is_upper(u8 c) {
    return ('A' <= c && c <= 'Z');
}

b8 char_is_lower(u8 c) {
    return ('a' <= c && c <= 'z');
}

b8 char_is_alphabet(u8 c) {
    return char_is_lower(c) || char_is_upper(c);
}

b8 char_is_digit(u8 c, u32 base) {
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

str8 _str8_create(u8* str, u64 size) {
    str8 result = { .str = str, .size = size };
    return result;
}

str8 str8_of_size(u64 size, Arena* arena) {
    assert(arena);
    str8 str = { .size = size };
    str.str = arena_push_array(arena, u8, size + 1);
    str.str[str.size] = 0;
    return str;
}

str8 str8_from_cstr(const u8* str) {
    str8 result = { .str = (u8*)str, .size = cstr8_length((u8*)str) };
    return result;
}

str8 str8_copy(const str8 str, Arena* arena) {
    assert(arena && str.str);
    str8 str_new = str8_of_size(str.size, arena);
    mem_copy(str_new.str, str.str, str.size);
    str.str[str.size] = 0;
    return str_new;
}

str8 str8_to_lower(const str8 str, Arena* arena) {
    str8 result = str8_copy(str, arena);
    for (u64 i = 0; i < result.size; i++)
        result.str[i] = char_to_lower(result.str[i]);
    return result;
}

str8 str8_to_upper(const str8 str, Arena* arena) {
    str8 result = str8_copy(str, arena);
    for (u64 i = 0; i < result.size; i++)
        result.str[i] = char_to_upper(result.str[i]);
    return result;
}

str8 str8_concat(const str8 a, const str8 b, Arena* arena) {
    str8 result = { .size = a.size + b.size };
    result.str = arena_push_array(arena, u8, result.size + 1);
    mem_copy(result.str, a.str, b.size);
    mem_copy(result.str + a.size, b.str, b.size);
    result.str[result.size] = 0;
    return result;
}

b8 str8_equal(const str8 a, const str8 b) {
    return (a.size == b.size) ? mem_compare(a.str, b.str, a.size) : 0;
}

u64 cstr16_length(u16* c) {
    u16* p = c;
    for (;*p != 0; p++);
    return (p - c);
}

str16 _str16_create(u16* str, u64 size) {
    str16 result = { .str = str, .size = size };
    return result;
}

str16 str16_of_size(u64 size, Arena* arena) {
    assert(arena);
    str16 str = { .size = size };
    str.str = arena_push_array(arena, u16, str.size + 1);
    str.str[str.size] = 0;
    return str;
}

str16 str16_from_cstr(const u16* str) {
    str16 result = { .str = (u16*)str, .size = cstr16_length((u16*)str) };
    return result;
}

str16 str16_copy(const str16 str, Arena* arena) {
    assert(arena && str.str);
    str16 str_new = str16_of_size(str.size, arena);
    mem_copy(str_new.str, str.str, str.size);
    str_new.str[str_new.size] = 0;
    return str_new;
}

#if defined(OS_WINDOWS)
#include <Windows.h>

SystemInfo* os_get_system_info(Arena* arena) {
    mstd_assert(arena);
    SystemInfo* s_info = arena_push_struct(arena, SystemInfo);
    if (!s_info) return NULL;

    SYSTEM_INFO sysinfo = { 0 };
    GetSystemInfo(&sysinfo);

    s_info->page_size = sysinfo.dwPageSize;
    s_info->large_page_size = GetLargePageMinimum();
    s_info->allocation_granularity = sysinfo.dwAllocationGranularity;
    s_info->processors = sysinfo.dwNumberOfProcessors;
    s_info->numa_nodes = 1; // Default
    s_info->cache_line_size = 64; // Safe default (x86/x64 standard)

    DWORD buffer_size = 0;
    GetLogicalProcessorInformationEx(RelationAll, NULL, &buffer_size);
    if (buffer_size > 0) arena_scratch(arena) {
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer =
            (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)arena_push(arena, buffer_size, alignof(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX));

        if (buffer && GetLogicalProcessorInformationEx(RelationAll, buffer, &buffer_size)) {
            u32 numa_count = 0;
            u64 max_cache_line = 0;

            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX current = buffer;
            DWORD bytes_processed = 0;

            while (bytes_processed < buffer_size) {
                switch (current->Relationship) {
                case RelationNumaNode:
                    numa_count++;
                    break;
                case RelationCache:
                    if (current->Cache.Type == CacheData || current->Cache.Type == CacheUnified) {
                        if (current->Cache.LineSize > max_cache_line) {
                            max_cache_line = current->Cache.LineSize;
                        }
                    }
                    break;
                }
                bytes_processed += current->Size;
                current = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((u8*)current + current->Size);
            }

            s_info->numa_nodes = numa_count > 0 ? numa_count : 1;
            s_info->cache_line_size = max_cache_line > 0 ? max_cache_line : 64;
        }
    }

    return s_info;
}

ProcessInfo* os_get_process_info(Arena* arena) {
    mstd_assert(arena);
    ProcessInfo* p_info = arena_push_struct(arena, ProcessInfo);
    if (!p_info) return NULL;

    p_info->pid = GetCurrentProcessId();
    p_info->group_id = 0;

    HANDLE h_proc = GetCurrentProcess();

    DWORD_PTR process_mask, system_mask;
    if (GetProcessAffinityMask(h_proc, &process_mask, &system_mask))
        p_info->affinity_mask = (u64)process_mask;
    else
        p_info->affinity_mask = (u64)system_mask;

    SIZE_T min_ws, max_ws;
    DWORD flags;
    if (GetProcessWorkingSetSizeEx(h_proc, &min_ws, &max_ws, &flags))
        p_info->page_file_limit = (u64)max_ws;
    else
        p_info->page_file_limit = (u64)-1;

    return p_info;
}

u64 os_get_page_size() {
    SYSTEM_INFO sysinfo = { 0 };
    GetSystemInfo(&sysinfo);
    return sysinfo.dwPageSize;
}

u64 os_get_large_page_size() {
    return GetLargePageMinimum();
}

void* os_reserve(u64 size) {
    void* result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

void* os_reserve_large(u64 size) {
    void* result = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
    return result;
}

b8 os_commit(void* ptr, u64 size) {
    b8 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != NULL);
    return result;
}

b8 os_commit_large(void* ptr, u64 size) {
    UNUSED(size); // because windows
    UNUSED(ptr); // because windows
    return 1;
}

void os_decommit(void* ptr, u64 size) {
    mstd_assert(ptr);
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

void os_release(void* ptr, u64 size) {
    UNUSED(size); // because windows
    VirtualFree(ptr, 0, MEM_RELEASE);
}

str16 str16_from_str8(str8 str, Arena* arena) {
    str16 result = str16_of_size(str.size, arena);
    MultiByteToWideChar(CP_UTF8, 0, (const char*)str.str, (int)str.size, result.str, (int)result.size);
    return result;
}

#endif

#endif // MSTD_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // MSTD_H
