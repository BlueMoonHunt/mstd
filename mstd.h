#if !defined(MSTD_H)
#define MSTD_H

// Compiler
#if defined(_MSC_VER)
#define COMPILER_MSVC 1
#else
#define COMPILER_MSVC 0

#endif

#if defined(__clang__)
#define COMPILER_CLANG 1
#else
#define COMPILER_CLANG 0
#endif

#if defined(__GNUC__)
#define COMPILER_GCC 1
#else
#define COMPILER_GCC 0
#endif

// OS
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

#if defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#define OS_MAC 0
#endif

// Architecture
#if defined(_M_AMD64) || defined(__x86_64__) || defined(__amd64__)
#define ARCH_X64 1
#else
#define ARCH_X64 0
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define ARCH_ARM64 1
#else
#define ARCH_ARM64 0
#endif

#if defined(_M_IX86) || defined(__i386__)
#define ARCH_X86 1
#else
#define ARCH_X86 0
#endif

#if defined(__arm__) || defined(_M_ARM)
#define ARCH_ARM 1
#else
#define ARCH_ARM 0
#endif

// Language
#if defined(__cplusplus)
#define LANG_CXX 1
#else
#define LANG_C 1
#endif

#include <math.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#if COMPILER_MSVC
#include <intrin.h>
#endif

////////////////////////////////
// Keywords, Tags

#if COMPILER_MSVC
#define force_inline __forceinline
#elif COMPILER_CLANG || COMPILER_GCC
#define force_inline inline __attribute__((always_inline))
#else
#error force_inline not defined for this compiler.
#endif

#if COMPILER_MSVC
#define no_inline __declspec(noinline)
#elif COMPILER_CLANG || COMPILER_GCC
#define no_inline __attribute__((noinline))
#else
#error no_inline not defined for this compiler.
#endif

#define global static

#if COMPILER_CLANG || COMPILER_GCC
#define internal static __attribute__((unused))
#elif COMPILER_MSVC
#define internal __pragma(warning(suppress: 4505)) static
#else
#define internal static
#endif

#define expose __declspec(dllexport) __stdcall

#if MSTD_DLL
#define export expose
#else
#define export
#endif

////////////////////////////////
// Utils

#define SYMBOL_TO_CSTR(S) #S
#define CSTR(S) SYMBOL_TO_CSTR(S)

#define JOIN_SYMBOLS(a, b) a##b
#define JOIN(a, b) JOIN_SYMBOLS(a, b)

#define scope(begin, end)                                                      \
    for (int scope_var_i = ((begin), 0); !scope_var_i; scope_var_i += 1, (end))

#define bit(x) (1ULL << x)

#define NPOS (u64)(-1)

#define OPTIONS(T, ...)                                                        \
    typedef struct T {                                                         \
        __VA_ARGS__                                                            \
    } T

#define STATIC_ASSERT _Static_assert

////////////////////////////////
// Base Types

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef float f32;
typedef double f64;

#if (ARCH_X64) || (ARCH_ARM64)
typedef long long iptr;
typedef unsigned long long uptr;
#else
typedef int iptr;
typedef unsigned int uptr;
#endif

////////////////////////////////
// Atomics

#define AtomicType _Atomic

typedef AtomicType(u8) atomic_u8;
typedef AtomicType(u16) atomic_u16;
typedef AtomicType(u32) atomic_u32;
typedef AtomicType(u64) atomic_u64;

typedef AtomicType(i8) atomic_i8;
typedef AtomicType(i16) atomic_i16;
typedef AtomicType(i32) atomic_i32;
typedef AtomicType(i64) atomic_i64;

////////////////////////////////
// Limits

#define u8_min 0
#define u16_min 0
#define u32_min 0
#define u64_min 0

#if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#define i8_min ((-__INT8_MAX__) - 1)
#define i8_max __INT8_MAX__
#define u8_max (__INT8_MAX__ * 2U + 1U)

#define i16_min ((-__INT16_MAX__) - 1)
#define i16_max __INT16_MAX__
#define u16_max (__INT16_MAX__ * 2U + 1U)

#define i32_min ((-__INT32_MAX__) - 1)
#define i32_max __INT32_MAX__
#define u32_max (__INT32_MAX__ * 2U + 1U)

#define i64_min ((-__INT64_MAX__) - 1LL)
#define i64_max __INT64_MAX__
#define u64_max (__INT64_MAX__ * 2ULL + 1ULL)

#define f32_min __FLT_MIN__
#define f32_max __FLT_MAX__
#define f64_min __DBL_MIN__
#define f64_max __DBL_MAX__

#else
#define i8_min ((i8)(-127 - 1))
#define i8_max ((i8)127)
#define u8_max ((u8)255U)

#define i16_min ((i16)(-32767 - 1))
#define i16_max ((i16)32767)
#define u16_max ((u16)65535U)

#define i32_min (-2147483647 - 1)
#define i32_max 2147483647
#define u32_max 4294967295U

#define i64_min (-9223372036854775807LL - 1LL)
#define i64_max 9223372036854775807LL
#define u64_max 18446744073709551615ULL

#define f32_max (*(const float *)(const unsigned int[]){0x7F7FFFFF})
#define f32_min (*(const float *)(const unsigned int[]){0x00800000})
#define f64_max                                                                \
    (*(const double *)(const unsigned long long[]){0x7FEFFFFFFFFFFFFFULL})
#define f64_min                                                                \
    (*(const double *)(const unsigned long long[]){0x0010000000000000ULL})
#endif

#define enum_t(enum, T) T

#if COMPILER_MSVC

internal force_inline u8 u32_count_zerol(u32 x);
internal force_inline u8 u64_count_zerol(u64 x);
internal force_inline u8 u32_count_zeror(u32 x);
internal force_inline u8 u64_count_zeror(u64 x);

internal force_inline i8 u32_msb(u32 x);
internal force_inline i8 u64_msb(u64 x);
internal force_inline i8 u32_lsb(u32 x);
internal force_inline i8 u64_lsb(u64 x);

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
    u64 val[1];
} Handle;

////////////////////////////////
// Module: Memory

#if COMPILER_MSVC
void *memmove(void *dest, const void *src, size_t count);
int memcmp(const void *buffer1, const void *buffer2, size_t count);
#pragma intrinsic(memcmp, memmove)
#define mem_set(p, byte, size) __stosb((u8 *)(p), (u8)(byte), (size))
#define mem_copy(dest, src, size) __movsb((u8 *)(dest), (u8 *)(src), (size))
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
#define mem_copy_struct(dest, src) mem_copy((dest), (src), sizeof(*(dest)))
#define mem_copy_array(dest, src, count)                                       \
    mem_copy((dest), (src), sizeof(*(dest)) * (count))
#define mem_move_struct(dest, src) mem_move((dest), (src), sizeof(*(dest)))
#define mem_move_array(dest, src, count)                                       \
    mem_move((dest), (src), sizeof(*(dest)) * (count))

#if COMPILER_MSVC
#define mem_align_of(T) __alignof(T)
#elif COMPILER_CLANG
#define mem_align_of(T) __alignof(T)
#elif COMPILER_GCC
#define mem_align_of(T) __alignof__(T)
#else
#error AlignOf not defined for this compiler.
#endif

#if COMPILER_MSVC
#define mem_align_to(x) __declspec(align(x))
#elif COMPILER_CLANG || COMPILER_GCC
#define mem_align_to(x) __attribute__((aligned(x)))
#else
#error AlignType not defined for this compiler.
#endif

////////////////////////////////
// MATH

#define is_pow2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
#define is_pow2_or_zero(x) (((x) & ((x) - 1)) == 0)

#define align_up_pow2(x, p) (((x) + ((p) - 1)) & ~((p) - 1))
#define align_down_pow2(x, p) ((x) & ~((p) - 1))

#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define GB(x) ((x) << 30)
#define TB(x) ((x) << 40)

#define clamp_top(val, high) (((val) < (high)) ? (val) : (high))
#define clamp_bottom(val, low) (((val) > (low)) ? (val) : (low))
#define clamp(val, low, high) (clamp_bottom(low, clamp_top(val, high)))

#define DECLARE_QUANT(TYPE)                                                    \
    force_inline internal TYPE quantize_f32_to_##TYPE(f32 val, f32 min,        \
                                                      f32 max);                \
    force_inline internal f32 dequantize_##TYPE##_to_f32(TYPE val, f32 min,    \
                                                         f32 max);

#define DEFINE_QUANT(TYPE)                                                     \
    force_inline internal TYPE quantize_f32_to_##TYPE(f32 val, f32 min,        \
                                                      f32 max) {               \
        if (max <= min)                                                        \
            return (TYPE)0;                                                    \
        f32 scaled = ((val - min) / (max - min)) *                             \
                         ((f32)TYPE##_max - (f32)TYPE##_min) +                 \
                     (f32)TYPE##_min;                                          \
        return (TYPE)clamp(roundf(scaled), (f32)TYPE##_min, (f32)TYPE##_max);  \
    }                                                                          \
    force_inline internal f32 dequantize_##TYPE##_to_f32(TYPE val, f32 min,    \
                                                         f32 max) {            \
        if (max <= min)                                                        \
            return min;                                                        \
        return ((f32)val - (f32)TYPE##_min) /                                  \
                   ((f32)TYPE##_max - (f32)TYPE##_min) * (max - min) +         \
               min;                                                            \
    }

DECLARE_QUANT(i8)
DECLARE_QUANT(i16)
DECLARE_QUANT(i32)
DECLARE_QUANT(u8)
DECLARE_QUANT(u16)
DECLARE_QUANT(u32)

////////////////////////////////
// Module: Debug

#if COMPILER_MSVC
#define trap() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
#define trap() __builtin_trap()
#endif

#if MSTD_DEBUG
#define debug_assert(condition)                                                \
    do {                                                                       \
        if (!(condition)) {                                                    \
            printf("ASSERT FAILED\nFILE: %s\nLINE: %d\nCONDITION: %s\n",       \
                   __FILE__, __LINE__, #condition);                            \
            trap();                                                            \
        }                                                                      \
    } while (0)

#define debug_assert_code(code, check)                                         \
    do {                                                                       \
        if ((code) != (check)) {                                               \
            printf("CODE MISMATCH\nFILE: %s\nLINE: %d\nEXPECTED: %s\nACTUAL: " \
                   "%s\n",                                                     \
                   __FILE__, __LINE__, #check, #code);                         \
            trap();                                                            \
        }                                                                      \
    } while (0)

#define debug_panic()                                                          \
    do {                                                                       \
        printf("PANIC\nFILE: %s\nLINE: %d\n", __FILE__, __LINE__);             \
        trap();                                                                \
    } while (0)
#else
#define debug_assert(condition)
#define debug_assert_code(code, check) ((void)(code))
#define debug_panic
#endif

internal void *mem_reserve(u64 size, u32 large_pages);
internal u8 mem_commit(void *ptr, u64 size, u32 large_pages);
internal void mem_decommit(void *ptr, u64 size);
internal void mem_release(void *ptr, u64 size);

internal u64 mem_page_size(void);
internal u64 mem_large_page_size(void);

////////////////////////////////
// Module: Arena

typedef struct ArenaTempNode {
    struct ArenaTempNode *next;
} ArenaTempNode;

typedef struct Arena {
    u64 cursor;
    u64 committed;
    u64 reserved;
    ArenaTempNode *temp_stack_tail;
    ArenaTempNode *temp_stack_head;
    u32 page_size;
    u32 can_commit_large_pages;

#if MSTD_DEBUG
    u32 code_line_of_alloc;
    char *code_file_of_alloc;
#endif
} Arena;

#define ARENA_HEADER_SIZE align_up_pow2(sizeof(Arena), 64)

#if !ARENA_DEFAULT_RESERVE_SIZE
#define ARENA_DEFAULT_RESERVE_SIZE MB(64)
#endif

OPTIONS(ArenaOpt, u8 large_pages;);
internal Arena *arena_alloc_opt(u64 reserve_size, char *file, u32 line,
                                       ArenaOpt opt);
#define arena_alloc(reserve_size, ...)                                         \
    arena_alloc_opt(reserve_size, __FILE__, __LINE__, (ArenaOpt){__VA_ARGS__})

internal void arena_release(Arena *arena);
internal void arena_reset(Arena *arena);

internal void *arena_push(Arena *arena, u64 size, u64 align);
#define arena_push_struct(arena, T)                                            \
    (T *)arena_push(arena, sizeof(T), mem_align_of(T))
#define arena_push_array(arena, T, count)                                      \
    (T *)arena_push(arena, sizeof(T) * (count), mem_align_of(T))

internal void arena_temp_push(Arena *arena);
internal void arena_temp_pop(Arena *arena);
internal void arena_temp_pop_all(Arena *arena);
#define arena_temp_scope(arena)                                                \
    scope(arena_temp_push(arena), arena_temp_pop(arena))

internal Arena *arena_scratch_alloc(void);
internal void arena_scratch_release(Arena *arena);
#define arena_scratch_scope(scratch)                                           \
    for (Arena *scratch = arena_scratch_alloc(); scratch != NULL;              \
         (arena_scratch_release(scratch), scratch = NULL))

////////////////////////////////
// DS: LinkList

#define dll_push_back_np(head, tail, node, next, prev)                         \
    ((head) == 0 ? ((head) = (tail) = (node), (node)->next = (node)->prev = 0) \
                 : ((node)->prev = (tail), (tail)->next = (node),              \
                    (tail) = (node), (node)->next = 0))

#define dll_remove_np(head, tail, node, next, prev)                            \
    ((head) == (node)                                                          \
         ? ((head) == (tail) ? ((head) = (tail) = 0)                           \
                             : ((head) = (head)->next, (head)->prev = 0))      \
         : ((tail) == (node) ? ((tail) = (tail)->prev, (tail)->next = 0)       \
                             : ((node)->next->prev = (node)->prev,             \
                                (node)->prev->next = (node)->next)))

#define sll_push_front_n(head, tail, node, next)                               \
    ((node)->next = (head), ((head) == 0 ? (tail) = (node) : 0),               \
     (head) = (node))

#define sll_push_back_n(head, tail, node, next)                                \
    ((node)->next = 0,                                                         \
     ((head) == 0 ? ((head) = (node)) : ((tail)->next = (node))),              \
     (tail) = (node))

#define sll_pop_front_n(head, tail, next)                                      \
    ((head) == (tail) ? ((head) = (tail) = 0) : ((head) = (head)->next))

#define sll_pop_back_n(head, tail, next)                                       \
    ((head) == (tail) ? ((head) = (tail) = 0) : ((head) = (head)->next))

#define dll_push_front_np(head, tail, node, next, prev)                        \
    dll_push_back_np(tail, head, node, prev, next)

#define dll_push_back(head, tail, node)                                        \
    dll_push_back_np(head, tail, node, next, prev)
#define dll_push_front(head, tail, node)                                       \
    dll_push_front_np(head, tail, node, next, prev)
#define dll_remove(head, tail, node) dll_remove_np(head, tail, node, next, prev)

#define sll_push_front(head, tail, node)                                       \
    sll_push_front_n(head, tail, node, next)
#define sll_push_back(head, tail, node) sll_push_back_n(head, tail, node, next)
#define sll_pop_front(head, tail) sll_pop_front_n(head, tail, next)
#define sll_pop_back(head, tail) sll_pop_back_n(head, tail, next)

#define sll_stack_push(head, tail, node) sll_push_front(head, tail, node)
#define sll_stack_pop(head, tail) sll_pop_front(head, tail)

#define sll_queue_push(head, tail, node) sll_push_back(head, tail, node)
#define sll_queue_pop(head, tail) sll_pop_front(head, tail)

//////////////////////////////
// DS: DArray

typedef struct DArrayHeader DArrayHeader;
struct DArrayHeader {
    u64 size;
};

#define darray_tag                                                             \
    union {                                                                    \
        u64 size;                                                              \
        DArrayHeader header;                                                   \
    };

typedef struct DArrayMetaData DArrayMetaData;
struct DArrayMetaData {
    u8 shift;
    u8 chunks_n;
    u64 el_size;
};

internal force_inline void *darray_handle(Arena *arena,
                                                 DArrayHeader *header,
                                                 DArrayMetaData meta,
                                                 u64 index);

////////////////////////////////
// Module:  String

typedef struct Str8 {
    u8 *data;
    u64 size;
} Str8;

// An alias for Str8 used explicitly to denote a borrowed view of a string.
// * Semantically, a `Str8View` indicates that the underlying memory is owned by
// someone else and its lifetime is tied to the original source.
// * Convert to an allocated, null-terminated `Str8` using `str8_copy`.
typedef Str8 Str8View;

typedef struct Str16 {
    u16 *data;
    u64 size;
} Str16;

typedef struct Str32 {
    u32 *data;
    u64 size;
} Str32;

typedef struct Str8Node {
    struct Str8Node *next;
    Str8 data;
} Str8Node;

typedef struct UnicodeDecode {
    u32 inc;
    u32 codepoint;
} UnicodeDecode;

typedef enum CharType {
    CHAR_TYPE_SPACE = (1 << 0),
    CHAR_TYPE_UPPER = (1 << 1),
    CHAR_TYPE_LOWER = (1 << 2),
    CHAR_TYPE_DIGIT10 = (1 << 3),
    CHAR_TYPE_DIGIT16 = (1 << 4)
} CharType;

mem_align_to(64) global const u8 ASCII_LUT[256] = {
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

////////////////////////////////
// Character classification & conversion

#define char_is_space(c) (ASCII_LUT[(u8)(c)] & CHAR_TYPE_SPACE)
#define char_is_upper(c) (ASCII_LUT[(u8)(c)] & CHAR_TYPE_UPPER)
#define char_is_lower(c) (ASCII_LUT[(u8)(c)] & CHAR_TYPE_LOWER)
#define char_is_alpha(c)                                                       \
    (ASCII_LUT[(u8)(c)] & (CHAR_TYPE_UPPER | CHAR_TYPE_LOWER))
#define char_is_alphanumeric(c)                                                \
    (ASCII_LUT[(u8)(c)] &                                                      \
     (CHAR_TYPE_UPPER | CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT10))
#define char_is_numeric(c) (ASCII_LUT[(u8)(c)] & CHAR_TYPE_DIGIT10)
#define char_is_numeric_hex(c) (ASCII_LUT[(u8)(c)] & CHAR_TYPE_DIGIT16)
#define char_is_slash(c) (c == '/' || c == '\\')

#define char_to_upper(c) ((u8)((u8)(c) ^ (char_is_lower(c) ? 0x20 : 0)))
#define char_to_lower(c) ((u8)((u8)(c) ^ (char_is_upper(c) ? 0x20 : 0)))

////////////////////////////////
// Unicode

internal UnicodeDecode utf8_decode(u8 *str, u64 max);
internal UnicodeDecode utf16_decode(u16 *str, u64 max);
internal u32 utf8_encode(u8 *str, u32 codepoint);
internal u32 utf16_encode(u16 *str, u32 codepoint);
internal u32 utf8_size(u32 cp);
internal u32 utf16_size(u32 cp);

////////////////////////////////
// String Constructor

#define str8_lit(S)                                                            \
    (Str8) { .size = sizeof(S) - 1, .data = (u8 *)S }
#define str8(str) str8_from_cstr((u8 *)str)
internal Str8 str8_from_cstr(u8 *str);
internal Str8 str8_from_fmt(Arena *arena, char *fmt, ...);
internal Str8 str8_from_16(Arena *arena, Str16 str);
internal Str8 str8_from_32(Arena *arena, Str32 str);
internal Str8 str8_from_mem_size(Arena *arena, u64 size);

internal Str16 str16_from_cstr(u16 *str);
#define str16 str16_from_cstr
internal Str16 str16_from_8(Arena *arena, Str8 str);
internal Str16 str16_from_mem_size(Arena *arena, u64 size);

internal Str32 str32_from_8(Arena *arena, Str8 str);

////////////////////////////////
// String Matching

OPTIONS(Str8MatchOpt, u8 case_insensitive; u8 slash_insensitive;);

internal u32 str8_match_opt(Str8 a, Str8 b, Str8MatchOpt opt);
#define str8_match(a, b, ...) str8_match_opt(a, b, (Str8MatchOpt){__VA_ARGS__})
#define str8_match(a, b, ...) str8_match_opt(a, b, (Str8MatchOpt){__VA_ARGS__})
internal u64 str8_find_opt(Str8 string, Str8 substring, u64 offset,
                                  Str8MatchOpt opt);
#define str8_find(string, substring, offset, ...)                              \
    str8_find_opt(string, substring, offset, (Str8MatchOpt){__VA_ARGS__})
internal u64 str8_find_reverse_opt(Str8 string, Str8 substring,
                                          u64 offset, Str8MatchOpt opt);
#define str8_find_reverse(string, substring, offset, ...)                      \
    str8_find(string, substring, offset, (Str8MatchOpt){__VA_ARGS__})

////////////////////////////////
// String concatinate and copy

internal Str8 str8_concat(Arena *arena, Str8 a, Str8 b);
internal Str8 str8_concat_args_till_str_npos(Arena *arena, ...);
#define str8_concat_n(arena, ...)                                              \
    str8_concat_args_till_str_npos(arena, __VA_ARGS__, (Str8){.size = NPOS})

internal Str8 str8_copy(Arena *arena, Str8 str);
internal char* str8_copy_to_cstr(Arena *arena, Str8 str);

////////////////////////////////
// String slicing
OPTIONS(Str8ViewOpt, Str8 delimiter; u8 postfix;);
internal Str8View str8_slice_opt(Str8View str, u64 pos, Str8ViewOpt opt);
#define str8_slice(str, pos, ...)                                              \
    str8_slice_opt(str, pos, (Str8ViewOpt){__VA_ARGS__})

////////////////////////////////
// Module Thread

#if COMPILER_MSVC
#define thread_var __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
#define thread_var __thread
#else
#error thread_var not defined for this compiler
#endif

mem_align_to(64) typedef struct ThreadPrimitive {
    u8 reserved[64];
} ThreadPrimitive;

typedef ThreadPrimitive Thread;
typedef ThreadPrimitive Mutex;
typedef ThreadPrimitive RWMutex;
typedef ThreadPrimitive CondVar;
typedef ThreadPrimitive Semaphore;
typedef ThreadPrimitive Barrier;

typedef void ThreadEntryPointFn(void *data);

////////////////////////////////
// Threads

internal void thread_attach(Thread *thread, ThreadEntryPointFn *func,
                                   void *data);
internal u32 thread_join(Thread *thread);
internal void thread_detach(Thread *thread);
internal void thread_sleep(u32 ms);
internal u32 thread_id(void);

////////////////////////////////
// Mutex

internal void mutex_init(Mutex *mutex);
internal void mutex_take(Mutex *mutex);
internal void mutex_drop(Mutex *mutex);
internal void mutex_destroy(Mutex *mutex);
#define mutex_scope(mutex) scope(mutex_take(mutex), mutex_drop(mutex));

////////////////////////////////
// Read write mutex

internal void rw_mutex_init(RWMutex *mutex);
internal void rw_mutex_take(RWMutex *mutex, u32 write_mode);
internal void rw_mutex_drop(RWMutex *mutex, u32 write_mode);
internal void rw_mutex_destroy(RWMutex *mutex);

////////////////////////////////
// Condition Variables

internal void cond_var_init(CondVar *var);
internal u32 cond_var_wait(CondVar *var, Mutex *mutex);
internal u32 cond_var_wait_rw(CondVar *var, RWMutex *mutex,
                                     u32 write_mode);
internal void cond_var_signal(CondVar *var);
internal void cond_var_broadcast(CondVar *var);
internal void cond_var_destroy(CondVar *var);

#define cond_var_wait_r(var, mutex) cond_var_wait_rw((var), (mutex), (0))
#define cond_var_wait_w(var, mutex) cond_var_wait_rw((var), (mutex), (1))

// ////////////////////////////////
// // Semaphore

internal void semaphore_init(Semaphore *semaphore, u32 initial_count,
                                    u32 max_count);
internal u32 semaphore_take(Semaphore *semaphore);
internal void semaphore_drop(Semaphore *semaphore);
internal void semaphore_destroy(Semaphore *semaphore);

// ////////////////////////////////
// // Barriers

internal void barrier_init(Barrier *barrier, u32 count);
internal void barrier_wait(Barrier *barrier);
internal void barrier_destroy(Barrier *barrier);

////////////////////////////////
// Hardware Atomics wait-on-address

internal void atomic_wait_u32(atomic_u32 *addr, u32 expected_value);
internal void atomic_wake_single(atomic_u32 *addr);
internal void atomic_wake_all(atomic_u32 *addr);

////////////////////////////////
// Module: Cmd

typedef enum CmdPiority {
    CMD_PRIORITY_NORMAL,
    CMD_PRIORITY_IDLE,
    CMD_PRIORITY_HIGH,
    CMD_PRIORITY_REALTIME
}CmdPiority;

internal void cmd_cli_alloc(u32 if_not_exists);

OPTIONS(CmdOpt, u8 no_reset; u8 hidden; u8 inherit_handles; u8 run_ditached;
        enum_t(CmdPiority, u8) priority;);
internal u32 cmd_run_opt(char* command, CmdOpt opt);
#define cmd_run(command, ...) cmd_run_opt(command, (CmdOpt){__VA_ARGS__})

////////////////////////////////
// Module: Clock

internal u64 clock_resolution_us(void);
internal u64 clock_ticks_now(void);

////////////////////////////////
// Module Timer
typedef struct Timer {
    u64 ticks;
    f32 delta;
    u64 resolution_us;
    f64 inverse_ticks_per_us;
} Timer;

internal Timer timer_start(void);
internal void timer_update(Timer *timer);
internal u64 timer_get_timestamp(Timer *timer);

////////////////////////////////
// Module: File

typedef Handle FileHandle;

typedef enum FileAccessFlag {
    FILE_ACCESS_FLAG_OPEN_EXISTING = 1 << 0,
    FILE_ACCESS_FLAG_OPEN_ALWAYS = 1 << 1,
    FILE_ACCESS_FLAG_SHARE_READ = 1 << 2,
    FILE_ACCESS_FLAG_SHARE_WRITE = 1 << 3,
    FILE_ACCESS_FLAG_EXECUTE = 1 << 5,
    FILE_ACCESS_FLAG_READ = 1 << 6,
    FILE_ACCESS_FLAG_WRITE = 1 << 7,
    FILE_ACCESS_FLAG_APPEND = 1 << 8
} FileAccessFlag;

// FileWatcher

typedef enum FileEventType {
    FILE_EVENT_TYPE_NULL,
    FILE_EVENT_TYPE_MODIFIED,
    FILE_EVENT_TYPE_ADDED,
    FILE_EVENT_TYPE_DELETED,
    FILE_EVENT_TYPE_RENAMED,
} FileEventType;

typedef struct FileEvent {
    Str8 file_name;
    enum_t(FileEventType, u8) type;
} FileEvent;

#if OS_WINDOWS
typedef struct Win32FileWatcher FileWatcher;
#elif OS_LINUX
typedef struct LinuxFileWatcher FileWatcher;
#endif

internal u32 file_delete(Str8 path);
internal u32 file_copy(Str8 src, Str8 dest);
internal u32 file_move(Str8 src, Str8 dest);
internal u32 file_exists(Str8 path);
internal u32 file_directory_exists(Str8 path);

internal FileHandle file_open(Str8 name, FileAccessFlag flags);
internal u64 file_size(FileHandle handle);
internal void file_close(FileHandle handle);

internal u8 *file_read_ex(Arena *arena, FileHandle handle, u64 offset,
                                 u64 size);
#define file_read(arena, handle) file_read_ex(arena, handle, 0, NPOS)
#define file_read_struct(arena, handle, T, offset)                             \
    (T *)file_read_ex(arena, handle, sizeof(T), offset)

internal void file_write_ex(FileHandle handle, void *data, u64 offset,
                                   u64 size);
#define file_write(handle, data) file_write(handle, data, 0, NPOS)
#define file_write_struct(handle, struct_ptr, offset)                          \
    file_write_ex(handle, struct_ptr, offset, sizeof(*struct_ptr))
#define file_write_string(handle, str)                                         \
    file_write_ex(handle, str.data, 0, str.size)

////////////////////////////////
// Module: File Watcher

internal FileWatcher *file_watcher_create(Arena *arena, Str8 path,
                                                 u32 watch_sub_directory);
internal FileEvent *file_watcher_poll_events(FileWatcher *watcher,
                                                    Arena *arena,
                                                    u32 timeout_ms,
                                                    u32 *out_count);
internal void file_watcher_destroy(FileWatcher *watcher);

////////////////////////////////
// Module: Lib
typedef Handle LibHandle;

internal LibHandle lib_load(Str8 name);
internal void lib_unload(LibHandle handle);
internal void *lib_get_symbol(LibHandle lib, Str8 name);

#endif // MSTD_H