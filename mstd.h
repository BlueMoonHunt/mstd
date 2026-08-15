#if !defined(MSTD_H)
#define MSTD_H

/* Compiler */
#if defined(__clang__)

#define COMPILER_CLANG 1
#define COMPILER_MSVC 0
#define COMPILER_GCC 0

#elif defined(_MSC_VER)

#define COMPILER_CLANG 0
#define COMPILER_MSVC 1
#define COMPILER_GCC 0

#elif defined(__GNUC__)

#define COMPILER_CLANG 0
#define COMPILER_MSVC 0
#define COMPILER_GCC 1

#else

#define COMPILER_CLANG 0
#define COMPILER_MSVC 0
#define COMPILER_GCC 0
#error "Unknown compiler!"

#endif

/* OS */
#if defined(_WIN32)
#define OS_WINDOWS 1
#else
#define OS_WINDOWS 0
#endif

#if defined(__linux__)
#define OS_LINUX 1
#else
#define OS_LINUX 0
#endif

#if defined(__ANDROID__)
#define OS_ANDROID 1
#else
#define OS_ANDROID 0
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#define OS_MAC 0
#endif

#if defined(__APPLE_CC__)
#define OS_MACINTOSH 1
#else
#define OS_MACINTOSH 0
#endif

/* Architecture */
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#define ARCH_X64 1
#else
#define ARCH_X64 0
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define ARCH_ARM64 1
#else
#define ARCH_ARM64 0
#endif

#if defined(_M_IX86) || defined(i386) || defined(__i386) || defined(__i386__)
#define ARCH_X86 1
#else
#define ARCH_X86 0
#endif

#if defined(__arm__) || defined(_M_ARM)
#define ARCH_ARM 1
#else
#define ARCH_ARM 0
#endif

/* Language */
#if defined(__cplusplus)
#define LANG_CPP 1
#define LANG_C 0
#else
#define LANG_CPP 0
#define LANG_C 1
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#if COMPILER_MSVC
#include <intrin.h>
#endif

#if OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#endif

/* Keywords */

#define global static

#if COMPILER_CLANG || COMPILER_GCC
#define internal static __attribute__((unused))
#elif COMPILER_MSVC
#define internal __pragma(warning(suppress : 4505)) static
#else
#define internal static
#endif

#if OS_WINDOWS
#define export extern __declspec(dllexport)
#else
#define export extern
#endif

#if COMPILER_MSVC
#pragma section(".ro", read)
#define read_only __declspec(allocate(".ro")) const
#elif COMPILER_GCC || COMPILER_CLANG
#define read_only __attribute__((section(".rodata"))) const
#else
#define read_only const
#endif

#if COMPILER_MSVC
#define thread_var __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
#define thread_var __thread
#else
#define thread_var
#endif

#if COMPILER_MSVC
#define force_inline __forceinline
#elif COMPILER_CLANG || COMPILER_GCC
#define force_inline __attribute__((always_inline))
#else
#define force_inline
#endif

#if COMPILER_MSVC
#define no_inline __declspec(noinline)
#elif COMPILER_CLANG || COMPILER_GCC
#define no_inline __attribute__((noinline))
#else
#define no_inline
#endif

#if COMPILER_MSVC
#define unreachable() __assume(0)
#elif COMPILER_CLANG || COMPILER_GCC
#define unreachable() __builtin_unreachable()
#else
#define unreachable()
#endif

/* Units */
#define BIT(x) (1ULL << (x))
#define bit(x) (1ULL << (x))
#define KB(n) (((U64)(n)) << 10)
#define MB(n) (((U64)(n)) << 20)
#define GB(n) (((U64)(n)) << 30)
#define TB(n) (((U64)(n)) << 40)

/* Bit Ops */
#define bit_unset(val, index) ((val) &= ~bit(index))
#define bit_set(val, index) ((val) |= bit(index))
#define bit_toggle(val, index) ((val) ^= bit(index))
#define bit_is_set(val, index) (((val) >> (index)) & 1ULL)

/* pow2 math */
#define is_pow2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
#define is_pow2_or_zero(x) (((x) & ((x) - 1)) == 0)

#define align_up_pow2(x, p) (((x) + ((p) - 1)) & ~((p) - 1))
#define align_down_pow2(x, p) ((x) & ~((p) - 1))

/* Clamp */
#define clamp_top(val, high) (((val) < (high)) ? (val) : (high))
#define clamp_bottom(val, low) (((val) > (low)) ? (val) : (low))
#define clamp(val, low, high) (clamp_bottom(low, clamp_top(val, high)))

/* Alignment */
#if COMPILER_MSVC
#define align_of(T) __alignof(T)
#elif COMPILER_CLANG
#define align_of(T) __alignof(T)
#elif COMPILER_GCC
#define align_of(T) __alignof__(T)
#else
#error align_of not defined for this compiler.
#endif

#if COMPILER_MSVC
#define align_to(x) __declspec(align(x))
#elif COMPILER_CLANG || COMPILER_GCC
#define align_to(x) __attribute__((aligned(x)))
#else
#error align_to not defined for this compiler.
#endif

/* Misc */
#define symbol_to_cstr_(S) #S
#define symbol_to_cstr(S) to_cstr_(S)

#define symbol_join_(a, b) A##B
#define symbol_join(a, b) symbol_join_(a, b)

#define swap(T, a, b)                                                                                                  \
    do {                                                                                                               \
        T temp_ab = a;                                                                                                 \
        a = b;                                                                                                         \
        b = temp_ab;                                                                                                   \
    } while (0)

#define NPOS (U64)(-1)
#define STRNPOS NPOS

/* Base Types */
typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef float F32;
typedef double F64;

typedef enum Type {
    TYPE_INTEGER = bit(7),
    TYPE_SIGNED = 1 << 4,
    TYPE_UNSIGNED = 2 << 4,
    TYPE_VOID = 0,

    TYPE_U8 = TYPE_INTEGER | TYPE_UNSIGNED | sizeof(U8),
    TYPE_U16 = TYPE_INTEGER | TYPE_UNSIGNED | sizeof(U16),
    TYPE_U32 = TYPE_INTEGER | TYPE_UNSIGNED | sizeof(U32),
    TYPE_U64 = TYPE_INTEGER | TYPE_UNSIGNED | sizeof(U64),

    TYPE_I8 = TYPE_INTEGER | TYPE_SIGNED | sizeof(I8),
    TYPE_I16 = TYPE_INTEGER | TYPE_SIGNED | sizeof(I16),
    TYPE_I32 = TYPE_INTEGER | TYPE_SIGNED | sizeof(I32),
    TYPE_I64 = TYPE_INTEGER | TYPE_SIGNED | sizeof(I64),

    TYPE_F32 = TYPE_SIGNED | sizeof(F32),
    TYPE_F64 = TYPE_SIGNED | sizeof(F64),
} Type;

#define enum_val(E,T) T /* To store enum in desired type while keeping it readable. */

/* Atomic */
#define AtomicType _Atomic

typedef AtomicType(U8) atomic_u8;
typedef AtomicType(U16) atomic_u16;
typedef AtomicType(U32) atomic_u32;
typedef AtomicType(U64) atomic_u64;

typedef AtomicType(I8) atomic_i8;
typedef AtomicType(I16) atomic_i16;
typedef AtomicType(I32) atomic_i32;
typedef AtomicType(I64) atomic_i64;

/* Limits */
#define U8_MIN 0
#define U16_MIN 0
#define U32_MIN 0
#define U64_MIN 0

#define u8_min 0
#define u16_min 0
#define u32_min 0
#define u64_min 0

#if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#define I8_MIN ((-__INT8_MAX__) - 1)
#define I8_MAX __INT8_MAX__
#define U8_MAX (__INT8_MAX__ * 2U + 1U)

#define I16_MIN ((-__INT16_MAX__) - 1)
#define I16_MAX __INT16_MAX__
#define U16_MAX (__INT16_MAX__ * 2U + 1U)

#define I32_MIN ((-__INT32_MAX__) - 1)
#define I32_MAX __INT32_MAX__
#define U32_MAX (__INT32_MAX__ * 2U + 1U)

#define I64_MIN ((-__INT64_MAX__) - 1LL)
#define I64_MAX __INT64_MAX__
#define U64_MAX (__INT64_MAX__ * 2ULL + 1ULL)

#define F32_MIN __FLT_MIN__
#define F32_MAX __FLT_MAX__

#define F64_MIN __DBL_MIN__
#define F64_MAX __DBL_MAX__

#else
#define I8_MIN ((I8)(-127 - 1))
#define I8_MAX ((I8)127)
#define U8_MAX ((U8)255U)

#define I16_MIN ((I16)(-32767 - 1))
#define I16_MAX ((I16)32767)
#define U16_MAX ((U16)65535U)

#define I32_MIN (-2147483647 - 1)
#define I32_MAX 2147483647
#define U32_MAX 4294967295U

#define I64_MIN (-9223372036854775807LL - 1LL)
#define I64_MAX 9223372036854775807LL
#define U64_MAX 18446744073709551615ULL

#define F32_MAX (*(const float *)(const unsigned int[]){0x7F7FFFFF})
#define F32_MIN (*(const float *)(const unsigned int[]){0x00800000})

#define F64_MAX (*(const double *)(const unsigned long long[]){0x7FEFFFFFFFFFFFFFULL})
#define F64_MIN (*(const double *)(const unsigned long long[]){0x0010000000000000ULL})
#endif

#define i8_min I8_MIN
#define i8_max I8_MAX
#define u8_max U8_MAX

#define i16_min I16_MIN
#define i16_max I16_MAX
#define u16_max U16_MAX

#define i32_min I32_MIN
#define i32_max I32_MAX
#define u32_max U32_MAX

#define i64_min I64_MIN
#define i64_max I64_MAX
#define u64_max U64_MAX

#define f32_min F32_MIN
#define f32_max F32_MAX

#define f64_min F64_MIN
#define f64_max F64_MAX

#if COMPILER_MSVC

internal force_inline I8 u32_count_zerol(U32 x);
internal force_inline I8 u64_count_zerol(U64 x);
internal force_inline I8 u32_count_zeror(U32 x);
internal force_inline I8 u64_count_zeror(U64 x);

internal force_inline I8 u32_msb(U32 x);
internal force_inline I8 u64_msb(U64 x);
internal force_inline I8 u32_lsb(U32 x);
internal force_inline I8 u64_lsb(U64 x);

#define u32_count_set_bits __popcnt
#define u64_count_set_bits __popcnt64

#elif COMPILER_CLANG || COMPILER_GCC

#define u32_msb(x) ((x) == 0 ? -1 : 31 - __builtin_clz(x))
#define u64_msb(x) ((x) == 0 ? -1 : 63 - __builtin_clzll(x))

#define u32_lsb(x) ((x) == 0 ? -1 : __builtin_ctz(x))
#define u64_lsb(x) ((x) == 0 ? -1 : __builtin_ctzll(x))

#define u32_count_set_bits __builtin_popcount
#define u64_count_set_bits __builtin_popcountll

#define u32_count_zerol __builtin_clz
#define u64_count_zerol __builtin_clzll
#define u32_count_zeror __builtin_ctz
#define u64_count_zeror __builtin_ctzll

#endif

typedef struct Handle {
    U64 val[1];
} Handle;

/* Memory */

typedef struct Buffer Buffer;
struct Buffer {
    U8* data;
    U64 size;
};

typedef struct Buffer_RO Buffer_RO;
struct Buffer_RO {
    const U8* data;
    U64 size;
};

#if COMPILER_MSVC
void* memmove(void* dest, const void* src, size_t count);
int memcmp(const void* buffer1, const void* buffer2, size_t count);
#pragma intrinsic(memcmp, memmove)

#define mem_set(p, byte, size) __stosb((U8*)(p), (U8)(byte), (size))
#define mem_copy(dest, src, size) __movsb((U8*)(dest), (U8*)(src), (size))
#define mem_move(dest, src, size) memmove((dest), (src), (size))
#define mem_match(a, b, size) (memcmp((a), (b), (size)) == 0)

#elif COMPILER_CLANG || COMPILER_GCC

#define mem_set(p, byte, size) __builtin_memset((p), (byte), (size))
#define mem_copy(dest, src, size) __builtin_memcpy((dest), (src), (size))
#define mem_move(dest, src, size) __builtin_memmove((dest), (src), (size))
#define mem_match(a, b, size) (__builtin_memcmp((a), (b), (size)) == 0)

#else
#error "mem_" functions not defined for this compiler.
#endif

#define mem_zero(mem, size) mem_set((mem), 0, (size))
#define mem_zero_struct(mem) mem_zero((mem), sizeof(*(mem)))
#define mem_zero_array(mem, count) mem_zero((mem), sizeof(*(mem)) * (count))

#define mem_copy_type(dest, src) mem_copy((dest), (src), sizeof(*(dest)))
#define mem_copy_array(dest, src, count) mem_copy((dest), (src), sizeof(*(dest)) * (count))

#define mem_move_struct(dest, src) mem_move((dest), (src), sizeof(*(dest)))
#define mem_move_array(dest, src, count) mem_move((dest), (src), sizeof(*(dest)) * (count))


#define mem_reserve os_memory_reserve /* requires initialized os state. */
#define mem_commit os_memory_commit /* requires initialized os state. */
#define mem_decommit os_memory_decommit /* requires initialized os state. */
#define mem_release os_memory_release /* requires initialized os state. */

/* Arena */
typedef struct ArenaTempNode {
    U64 cursor;
    struct ArenaTempNode* next;
} ArenaTempNode;

typedef struct Arena {
    U64 cursor;
    U64 committed;
    U64 reserved;
    ArenaTempNode* temp_stack_tail;
    ArenaTempNode* temp_stack_head;
    U32 page_size;
    U32 can_commit_large_pages;
} Arena;

#define ARENA_HEADER_SIZE align_up_pow2(sizeof(Arena), 64)

#ifndef ARENA_DEFAULT_RESERVE_SIZE
#define ARENA_DEFAULT_RESERVE_SIZE MB(100)
#endif

#ifndef ARENA_DEFAULT_COMMIT_SIZE
#define ARENA_DEFAULT_COMMIT_SIZE KB(40)
#endif

#ifndef ARENA_DEFAULT_SCRATCH_COUNT
#define ARENA_DEFAULT_SCRATCH_COUNT 4
#endif

internal Arena* arena_alloc(U64 reserve_size, U64 commit_size, U8 enable_large_pages);
internal void* arena_push(Arena* arena, U64 size, U64 alignment);
internal void arena_release(Arena* arena);
internal void arena_reset(Arena* arena);

#define arena_alloc_default() arena_alloc(ARENA_DEFAULT_RESERVE_SIZE, ARENA_DEFAULT_COMMIT_SIZE, 0)
#define arena_push_type(arena, T) arena_push(arena, sizeof(T), align_of(T))
#define arena_push_array(arena, T, count) arena_push(arena, sizeof(T) * count, align_of(T))

internal void arena_temp_begin(Arena* arena);
internal void arena_temp_end(Arena* arena);
internal void arena_temp_end_all(Arena* arena);

internal Arena* arena_scratch_begin(void);
internal void arena_scratch_end(Arena* scratch_arena);

#define arena_scratch_scope(arena) for (arena = arena_scratch_begin(); arena; arena_scratch_end(arena), arena = 0)

typedef struct UnicodeDecode {
    U32 inc;
    U32 codepoint;
} UnicodeDecode;

/* Unicode */
internal UnicodeDecode utf8_decode(U8* str, U64 max);
internal UnicodeDecode utf16_decode(U16* str, U64 max);
internal U32 utf8_encode(U8* str, U32 codepoint);
internal U32 utf16_encode(U16* str, U32 codepoint);
internal U32 utf8_size(U32 cp);
internal U32 utf16_size(U32 cp);

typedef struct Str8 Str8;
typedef struct Str16 Str16;
typedef struct Str32 Str32;

/* null terminated */
typedef struct Str8 Str8;
struct Str8 {
    U8* data;
    U64 size;
};

/* not necessary to be null terminated */
typedef struct Str8 Str8Slice;

typedef enum CharType {
    CHAR_TYPE_SPACE = (1 << 0),
    CHAR_TYPE_UPPER = (1 << 1),
    CHAR_TYPE_LOWER = (1 << 2),
    CHAR_TYPE_DIGIT10 = (1 << 3),
    CHAR_TYPE_DIGIT16 = (1 << 4)
} CharType;

align_to(64) global const U8 ASCII_LUT[256] = {
    // White-space: Tab, LF, VT, FF, CR, Space
    [0x09] = CHAR_TYPE_SPACE,
    [0x0A] = CHAR_TYPE_SPACE,
    [0x0B] = CHAR_TYPE_SPACE,
    [0x0C] = CHAR_TYPE_SPACE,
    [0x0D] = CHAR_TYPE_SPACE,
    [0x20] = CHAR_TYPE_SPACE,

    // Digits: 0-9 (Decimal + Hex)
    ['0'] = CHAR_TYPE_DIGIT10 | CHAR_TYPE_DIGIT16,
    ['1'] = CHAR_TYPE_DIGIT10 | CHAR_TYPE_DIGIT16,
    ['2'] = CHAR_TYPE_DIGIT10 | CHAR_TYPE_DIGIT16,
    ['3'] = CHAR_TYPE_DIGIT10 | CHAR_TYPE_DIGIT16,
    ['4'] = CHAR_TYPE_DIGIT10 | CHAR_TYPE_DIGIT16,
    ['5'] = CHAR_TYPE_DIGIT10 | CHAR_TYPE_DIGIT16,
    ['6'] = CHAR_TYPE_DIGIT10 | CHAR_TYPE_DIGIT16,
    ['7'] = CHAR_TYPE_DIGIT10 | CHAR_TYPE_DIGIT16,
    ['8'] = CHAR_TYPE_DIGIT10 | CHAR_TYPE_DIGIT16,
    ['9'] = CHAR_TYPE_DIGIT10 | CHAR_TYPE_DIGIT16,

    // Uppercase Hex: A-F
    ['A'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT16,
    ['B'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT16,
    ['C'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT16,
    ['D'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT16,
    ['E'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT16,
    ['F'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT16,

    // Uppercase Non-Hex: G-Z
    ['G'] = CHAR_TYPE_UPPER,
    ['H'] = CHAR_TYPE_UPPER,
    ['I'] = CHAR_TYPE_UPPER,
    ['J'] = CHAR_TYPE_UPPER,
    ['K'] = CHAR_TYPE_UPPER,
    ['L'] = CHAR_TYPE_UPPER,
    ['M'] = CHAR_TYPE_UPPER,
    ['N'] = CHAR_TYPE_UPPER,
    ['O'] = CHAR_TYPE_UPPER,
    ['P'] = CHAR_TYPE_UPPER,
    ['Q'] = CHAR_TYPE_UPPER,
    ['R'] = CHAR_TYPE_UPPER,
    ['S'] = CHAR_TYPE_UPPER,
    ['T'] = CHAR_TYPE_UPPER,
    ['U'] = CHAR_TYPE_UPPER,
    ['V'] = CHAR_TYPE_UPPER,
    ['W'] = CHAR_TYPE_UPPER,
    ['X'] = CHAR_TYPE_UPPER,
    ['Y'] = CHAR_TYPE_UPPER,
    ['Z'] = CHAR_TYPE_UPPER,

    // Lowercase Hex: a-f
    ['a'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT16,
    ['b'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT16,
    ['c'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT16,
    ['d'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT16,
    ['e'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT16,
    ['f'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT16,

    // Lowercase Non-Hex: g-z
    ['g'] = CHAR_TYPE_LOWER,
    ['h'] = CHAR_TYPE_LOWER,
    ['i'] = CHAR_TYPE_LOWER,
    ['j'] = CHAR_TYPE_LOWER,
    ['k'] = CHAR_TYPE_LOWER,
    ['l'] = CHAR_TYPE_LOWER,
    ['m'] = CHAR_TYPE_LOWER,
    ['n'] = CHAR_TYPE_LOWER,
    ['o'] = CHAR_TYPE_LOWER,
    ['p'] = CHAR_TYPE_LOWER,
    ['q'] = CHAR_TYPE_LOWER,
    ['r'] = CHAR_TYPE_LOWER,
    ['s'] = CHAR_TYPE_LOWER,
    ['t'] = CHAR_TYPE_LOWER,
    ['u'] = CHAR_TYPE_LOWER,
    ['v'] = CHAR_TYPE_LOWER,
    ['w'] = CHAR_TYPE_LOWER,
    ['x'] = CHAR_TYPE_LOWER,
    ['y'] = CHAR_TYPE_LOWER,
    ['z'] = CHAR_TYPE_LOWER,
};

/* Character classification & conversion */
#define char_is_space(c) (ASCII_LUT[(U8)(c)] & CHAR_TYPE_SPACE)
#define char_is_upper(c) (ASCII_LUT[(U8)(c)] & CHAR_TYPE_UPPER)
#define char_is_lower(c) (ASCII_LUT[(U8)(c)] & CHAR_TYPE_LOWER)
#define char_is_alpha(c) (ASCII_LUT[(U8)(c)] & (CHAR_TYPE_UPPER | CHAR_TYPE_LOWER))
#define char_is_alphanumeric(c) (ASCII_LUT[(U8)(c)] & (CHAR_TYPE_UPPER | CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT10))
#define char_is_numeric(c) (ASCII_LUT[(U8)(c)] & CHAR_TYPE_DIGIT10)
#define char_is_numeric_hex(c) (ASCII_LUT[(U8)(c)] & CHAR_TYPE_DIGIT16)
#define char_is_slash(c) (c == '/' || c == '\\')

#define char_to_upper(c) ((U8)((U8)(c) ^ (char_is_lower(c) ? 0x20 : 0)))
#define char_to_lower(c) ((U8)((U8)(c) ^ (char_is_upper(c) ? 0x20 : 0)))

/* String Constructors */
#define str8_lit(S) (Str8){.size = (sizeof(S) / sizeof((*S))) - 1, .data = (U8*)(S)}
#define str8(S) str8_from_cstr(S)

internal Str8 str8_from_cstr(char* str);
internal Str8 str8_from_fmt(Arena* arena, char* fmt, ...);
internal Str8 str8_from_16(Arena* arena, Str16 str);
internal Str8 str8_from_32(Arena* arena, Str32 str);
internal Str8 str8_from_mem_size(Arena* arena, U64 size);
internal Str8 str8_from_slice(Arena* arena, Str8Slice slice);

/* String Matching */
typedef enum Str8MatchFlag {
    STR8_MATCH_FLAG_NONE = 0,
    STR8_MATCH_FLAG_CASE_INSENSITIVE = 1,
    STR8_MATCH_FLAG_SLASH_INSENSITIVE = 2,
    STR8_MATCH_FLAG_COUNT,
}Str8MatchFlag;

internal U32 str8_match(Str8 a, Str8 b, enum_val(Str8MatchFlag, U32) flags);
internal U64 str8_find(Str8 target, Str8 query, U64 offset, enum_val(Str8MatchFlag, U32) flags);
internal U64 str8_find_reverse(Str8 target, Str8 query, U64 offset, enum_val(Str8MatchFlag, U32) flags);

/* String concatinate and copy */
internal Str8 str8_concat(Arena* arena, Str8 a, Str8 b);
internal Str8 str8_concat_n_(Arena* arena, ...);

#define str8_concat_n(arena, ...) str8_concat_n_(arena, __VA_ARGS__, (Str8){.size = NPOS})

internal Str8 str8_copy(Arena* arena, Str8 str);
internal char* str8_copy_to_cstr(Arena* arena, Str8 str);

/* String slicing */
internal Str8Slice str8_slice(Str8Slice str, U64 pos, U8 give_postfix);
internal Str8Slice str8_slice_head_until(Str8Slice str, Str8 delimiter, U8 give_postfix);
internal Str8Slice str8_slice_tail_until(Str8Slice str, Str8 delimiter, U8 give_postfix);

typedef struct Str16 {
    U16* data;
    U64 size;
} Str16;

/* String Constructors */
#define str16_lit(S) ((Str16){ .size = (sizeof(S) / sizeof((*S))) - 1, .data = (S) })
#define str16(S) str16_from_cstr(S)

internal Str16 str16_from_cstr(U16* str);
internal Str16 str16_from_8(Arena* arena, Str8 str);
internal Str16 str16_from_mem_size(Arena* arena, U64 size);

typedef struct Str32 {
    U32* data;
    U64 size;
} Str32;

/* String Constructors */
#define str32_lit(S) ((Str32){ .size = (sizeof(S) / sizeof((*S))) - 1, .data = (S) })
#define str32(S) str32_from_cstr(S)

internal Str32 str32_from_cstr(U32* str);
internal Str32 str32_from_8(Arena* arena, Str8 str);
internal Str32 str32_from_mem_size(Arena* arena, U64 size);

/* OS State */

typedef struct SystemInfo SystemInfo;
struct SystemInfo {
    U64 logical_processor_count;
    U64 page_size;
    U64 large_page_size;
    U64 allocation_granularity;
    U64 microsecond_resolution;
};

typedef struct ProcessInfo ProcessInfo;
struct ProcessInfo {
    U32 id;
    Str8 bin_path;
    Str8 current_working_directory;
};

typedef struct OSState OSState;
struct OSState {
    Arena* arena;
    SystemInfo system_info;
    ProcessInfo process_info;
};

global OSState os_state;

/* --- os state --- */
internal SystemInfo* os_get_system_info(void);
internal ProcessInfo* os_get_process_info(void);
internal Str8 os_get_current_working_directory(Arena* arena);
internal U64 os_ticks_now(void);
internal void os_state_init(void);

/* --- os memory --- */
internal void* os_memory_reserve(U64 size, U64 large_pages);
internal void os_memory_commit(void* ptr, U64 size, U64 large_pages);
internal void os_memory_decommit(void* ptr, U64 size);
internal void os_memory_release(void* ptr, U64 size);

/* Thread Primitives */
align_to(64) typedef struct ThreadPrimitive {
    U8 reserved[64];
} ThreadPrimitive;

typedef ThreadPrimitive Thread;
typedef ThreadPrimitive Mutex;
typedef ThreadPrimitive RWMutex;
typedef ThreadPrimitive CondVar;
typedef ThreadPrimitive Semaphore;
typedef ThreadPrimitive Barrier;

typedef void ThreadEntryPointFn(void* data);

/* Threads */

internal void os_thread_attach(Thread* thread, ThreadEntryPointFn* func, void* data);
internal U32 os_thread_join(Thread* thread);
internal void os_thread_detach(Thread* thread);
internal void os_thread_sleep(U32 ms);
internal U32 os_thread_id(void);

/* Mutex */

internal void  os_mutex_init(Mutex* mutex);
internal void  os_mutex_take(Mutex* mutex);
internal void  os_mutex_drop(Mutex* mutex);
internal void  os_mutex_destroy(Mutex* mutex);

/* Read write mutex */

internal void os_rw_mutex_init(RWMutex* mutex);
internal void os_rw_mutex_take(RWMutex* mutex, U32 write_mode);
internal void os_rw_mutex_drop(RWMutex* mutex, U32 write_mode);
internal void os_rw_mutex_destroy(RWMutex* mutex);

/* Condition Variables */

internal void os_cond_var_init(CondVar* var);
internal U32 os_cond_var_wait(CondVar* var, Mutex* mutex);
internal U32 os_cond_var_wait_rw(CondVar* var, RWMutex* mutex, U32 write_mode);
internal void os_cond_var_signal(CondVar* var);
internal void os_cond_var_broadcast(CondVar* var);
internal void os_cond_var_destroy(CondVar* var);

#define os_cond_varwait_r(var, mutex) os_cond_var_wait_rw((var), (mutex), (0))
#define os_cond_varwait_w(var, mutex) os_cond_var_wait_rw((var), (mutex), (1))

/* Semaphore */

internal void  os_semaphore_init(Semaphore* semaphore, U32 initial_count, U32 max_count);
internal U32  os_semaphore_take(Semaphore* semaphore);
internal void  os_semaphore_drop(Semaphore* semaphore);
internal void  os_semaphore_destroy(Semaphore* semaphore);

/* Barriers */

internal void os_barrier_init(Barrier* barrier, U32 count);
internal void os_barrier_wait(Barrier* barrier);
internal void os_barrier_destroy(Barrier* barrier);

/* Hardware Atomics wait-on-address */

internal void os_atomic_wait_u32(atomic_u32* addr, U32 expected_value);
internal void os_atomic_wake_single(atomic_u32* addr);
internal void os_atomic_wake_all(atomic_u32* addr);

/* Lib loader */

internal Handle os_lib_load(Str8 name);
internal void os_lib_unload(Handle handle);
internal void* os_lib_get_symbol(Handle lib, char* name);

#define lib_load os_lib_load
#define lib_unload os_lib_unload
#define lib_get_symbol os_lib_get_symbol

/* File ops */

typedef enum FileAccessFlag {
    FILE_ACCESS_FLAG_OPEN_EXISTING  = 1 << 0,
    FILE_ACCESS_FLAG_OPEN_ALWAYS    = 1 << 1,
    FILE_ACCESS_FLAG_SHARE_READ     = 1 << 2,
    FILE_ACCESS_FLAG_SHARE_WRITE    = 1 << 3,
    FILE_ACCESS_FLAG_EXECUTE        = 1 << 5,
    FILE_ACCESS_FLAG_READ           = 1 << 6,
    FILE_ACCESS_FLAG_WRITE          = 1 << 7,
    FILE_ACCESS_FLAG_APPEND         = 1 << 8
} FileAccessFlag;

#define OS_FILE_OP_MAX_RETRIES 5

internal U32 os_file_delete(Str8 path);
internal U32 os_file_copy(Str8 src, Str8 dest);
internal U32 os_file_move(Str8 src, Str8 dest);
internal U32 os_file_exists(Str8 path);
internal U32 os_file_directory_exists(Str8 path);

internal Handle os_file_open(Str8 name, FileAccessFlag flags);
internal U64 os_file_size(Handle handle);
internal void os_file_close(Handle handle);
internal Str8 os_file_read(Arena* arena, Handle handle, U64 offset, U64 size);
internal void os_file_write(Handle handle, void* data, U64 offset, U64 size);


#define file_delete os_file_delete
#define file_copy os_file_copy
#define file_move os_file_move
#define file_exists os_file_exists
#define file_directory_exists os_file_directory_exists

#define file_open os_file_open
#define file_close os_file_close
#define file_size os_file_size
#define file_read(arena, handle) os_file_read(arena, handle, 0, NPOS)
#define file_read_struct(arena, handle, T, offset) (T*)os_file_read(arena, handle, sizeof(T), offset)

#define file_write(handle, data) os_file_write(handle, data, 0, NPOS)
#define file_write_struct(handle, struct_ptr, offset) os_file_write(handle, struct_ptr, offset, sizeof(*struct_ptr))
#define file_write_string(handle, str) os_file_write(handle, str.data, 0, str.size)

/* FileWatcher */

typedef enum FileEventType {
    FILE_EVENT_TYPE_NULL,
    FILE_EVENT_TYPE_MODIFIED,
    FILE_EVENT_TYPE_ADDED,
    FILE_EVENT_TYPE_DELETED,
    FILE_EVENT_TYPE_RENAMED,
} FileEventType;

typedef struct FileEvent {
    Str8 file_name;
    enum_val(FileEventType, U8) type;
} FileEvent;

#if OS_WINDOWS

typedef struct Win32FileWatcher FileWatcher;
struct Win32FileWatcher {
    HANDLE dir_handle;
    HANDLE iocp;
    U8 notification_buffer[KB(8)];
    OVERLAPPED overlapped;
    U32 scan_sub_directories;
};

#endif

internal FileWatcher* os_file_watcher_create(Arena* arena, Str8 path, U32 watch_sub_directory);
internal FileEvent* os_file_watcher_poll_events(FileWatcher* watcher, Arena* arena, U32 timeout_ms, U32* out_count);
internal void os_file_watcher_destroy(FileWatcher* watcher);

#define file_watcher_create os_file_watcher_create
#define file_watcher_poll_events os_file_watcher_poll_events
#define file_watcher_destroy os_file_watcher_destroy

/* CMD */

typedef enum CmdFlag {
    CMD_FLAG_PRIORITY_NORMAL    = bit(0),
    CMD_FLAG_PRIORITY_IDLE      = bit(1),
    CMD_FLAG_PRIORITY_HIGH      = bit(2),
    CMD_FLAG_PRIORITY_REALTIME  = bit(3),
    CMD_FLAG_HIDDEN             = bit(4),
    CMD_FLAG_INHERIT_HANDLE     = bit(5),
    CMD_FLAG_RUN_DITACHED       = bit(6)
} CmdFlag;

internal void os_cli_alloc(U32 if_not_exists);

internal U32 os_cmd(char* command, enum_val(CmdFlag, U8) flags);
internal void os_cmd_line_args(Arena* arena, I32* argc, Str8** argv);

#define cmd(command)            os_cmd((command), CMD_FLAG_PRIORITY_NORMAL)
#define cmd_background(command) os_cmd((command), CMD_FLAG_RUN_DITACHED | CMD_FLAG_HIDDEN | CMD_FLAG_PRIORITY_LOW)
#define cmd_ex(command, flags)  os_cmd((command), (flags))

#define cmd_line_args(arena, argc, argv) os_cmd_line_args(arena, &argc, &argv)

/* Timer */

typedef struct Timer {
    U64 ticks;
    F32 delta;
    U64 resolution_us;
    F64 inverse_ticks_per_us;
} Timer;

internal Timer timer_start(void);
internal void timer_update(Timer* timer);
internal U64 timer_get_timestamp(Timer* timer);

/* trace */

#if !TRACE
#define TRACE 0
#endif

typedef enum TraceLevel {
    TRACE_LEVEL_INFO,
    TRACE_LEVEL_WARNING,
    TRACE_LEVEL_ERROR,
}TraceLevel;

global Handle trace_file; /* file handle for trace file */
global U8 trace_is_hidden; /* trace will not be send to cmd */

#define trace_file_set(file_handle) trace_file = file_handle
#define trace_hide() trace_is_hidden = 1
#define trace_is_active() TRACE

internal void trace_(TraceLevel level, Str8 message);
#define trace(level, message)       \
    do {                            \
        if (TRACE)                  \
            trace_(level, message); \
    } while (0)

/* Debug */
typedef U32 (debug_callback)(void* user_data);

/* DArray */

typedef struct DArrayHeader DArrayHeader;
struct DArrayHeader {
    U64 size;
};

#define darray_tag                                                                                                     \
    union {                                                                                                            \
        U64 size;                                                                                                      \
        DArrayHeader header;                                                                                           \
    };

typedef struct DArrayMetaData DArrayMetaData;
struct DArrayMetaData {
    U8 shift;
    U8 chunks_max;
    U32 el_size;
};

internal force_inline void* darray_handle(Arena* arena, DArrayHeader* header, DArrayMetaData meta, U64 index);

/* LinkList */
#define dll_push_back_np(head, tail, node, next, prev)                                                                 \
    ((head) == 0 ? ((head) = (tail) = (node), (node)->next = (node)->prev = 0)                                         \
                 : ((node)->prev = (tail), (tail)->next = (node), (tail) = (node), (node)->next = 0))

#define dll_remove_np(head, tail, node, next, prev)                                                                    \
    ((head) == (node) ? ((head) == (tail) ? ((head) = (tail) = 0) : ((head) = (head)->next, (head)->prev = 0))         \
                      : ((tail) == (node) ? ((tail) = (tail)->prev, (tail)->next = 0)                                  \
                                          : ((node)->next->prev = (node)->prev, (node)->prev->next = (node)->next)))

#define sll_push_front_n(head, tail, node, next)                                                                       \
    ((node)->next = (head), ((head) == 0 ? (tail) = (node) : 0), (head) = (node))

#define sll_push_back_n(head, tail, node, next)                                                                        \
    ((node)->next = 0, ((head) == 0 ? ((head) = (node)) : ((tail)->next = (node))), (tail) = (node))

#define sll_pop_front_n(head, tail, next) ((head) == (tail) ? ((head) = (tail) = 0) : ((head) = (head)->next))
#define sll_pop_back_n(head, tail, next) ((head) == (tail) ? ((head) = (tail) = 0) : ((head) = (head)->next))

#define dll_push_front_np(head, tail, node, next, prev) dll_push_back_np(tail, head, node, prev, next)

#define dll_push_back(head, tail, node) dll_push_back_np(head, tail, node, next, prev)
#define dll_push_front(head, tail, node) dll_push_front_np(head, tail, node, next, prev)
#define dll_remove(head, tail, node) dll_remove_np(head, tail, node, next, prev)

#define sll_push_front(head, tail, node) sll_push_front_n(head, tail, node, next)
#define sll_push_back(head, tail, node) sll_push_back_n(head, tail, node, next)
#define sll_pop_front(head, tail) sll_pop_front_n(head, tail, next)
#define sll_pop_back(head, tail) sll_pop_back_n(head, tail, next)
#define sll_stack_push(head, tail, node) sll_push_front(head, tail, node)
#define sll_stack_pop(head, tail) sll_pop_front(head, tail)
#define sll_queue_push(head, tail, node) sll_push_back(head, tail, node)
#define sll_queue_pop(head, tail) sll_pop_front(head, tail)

#endif /* MSTD_H */