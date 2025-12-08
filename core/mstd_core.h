#ifndef TYPES_IMPLEMENTATION
#define TYPES_IMPLEMENTATION
#include <stdint.h>
#include <assert.h>

#if defined(_MSC_VER)
    #define COMPILER_MSVC 1
    #define COMPILER_GCC 0
    #define COMPILER_CLANG 0
#elif defined(__clang__)
    #define COMPILER_MSVC 0
    #define COMPILER_GCC 0
    #define COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define COMPILER_MSVC 0
    #define COMPILER_GCC 1
    #define COMPILER_CLANG 0
#else
    #define COMPILER_MSVC 0
    #define COMPILER_GCC 0
    #define COMPILER_CLANG 0
    #warning "Unknown Compiler"
#endif

#if defined(_WIN32)
    #define OS_WINDOWS 1
    #define OS_LINUX 0
    #define OS_MAC 0
#elif defined(__linux__)
    #define OS_WINDOWS 0
    #define OS_LINUX 1
    #define OS_MAC 0
#elif defined(__APPLE__) && defined(__MACH__)
    #define OS_WINDOWS 0
    #define OS_LINUX 0
    #define OS_MAC 1
#else
    #define OS_WINDOWS 0
    #define OS_LINUX 0
    #define OS_MAC 0
    #warning "Unknown Operating System"
#endif

#if defined(_M_AMD64) || defined(__amd64__) || defined(__x86_64__)
    #define ARCH_X64 1
    #define ARCH_X86 0
    #define ARCH_ARM 0
#elif defined(_M_IX86) || defined(__i386__)
    #define ARCH_X64 0
    #define ARCH_X86 1
    #define ARCH_ARM 0
#elif defined(_M_ARM) || defined(__arm__) || defined(__aarch64__)
    #define ARCH_X64 0
    #define ARCH_X86 0
    #define ARCH_ARM 1
#else
    #define ARCH_X64 0
    #define ARCH_X86 0
    #define ARCH_ARM 0
    #warning "Unknown Architecture"
#endif

////////////////////////////////
// types

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

typedef float f32;
typedef double f64;

#define enum_class(type, size_type) size_type

////////////////////////////////
// math and utils
#define align_as _Alignas
#define align_of _Alignof
#define align_up_pow2(x,b)   (((x) + (b) - 1)&(~((b) - 1)))
#define align_down_pow2(x,b) ((x)&(~((b) - 1)))
#define align_up_pad_pow2(x,b)  ((0-(x)) & ((b) - 1))
#define is_pow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define is_pow2_or_0(x)    ((((x) - 1)&(x)) == 0)

#define extract_bit(word, idx) (((word) >> (idx)) & 1)
#define extract8(word, pos)   (((word) >> ((pos) << 3)) & UINT8_MAX)
#define extract16(word, pos)  (((word) >> ((pos) << 4)) & UINT16_MAX)
#define extract32(word, pos)  (((word) >> ((pos) << 5)) & UINT32_MAX)

#define bit_rotate(value, rotation) ((value >> rotation) | (value << (-rotation) & (sizeof(value) * 8 - 1)));

#define KB(value) ((value) << 10)
#define MB(value) ((value) << 20)
#define GB(value) ((value) << 30)
#define TB(value) ((value) << 40)

#define max(a,b) (((a) > (b))? a : b)
#define min(a,b) (((a) < (b))? a : b)
#define clamp(low,high,value) max(low,min(value,high))

////////////////////////////////
// memory

b32 mem_is_pow2(uaddress address);
b32 mem_is_aligned(void* data, u64 alignment);
b32 mem_compare(void* data1, void* data2, u64 count);
void mem_copy(void* destination, void* source, u64 size);
void mem_set(void* data, u8 value, u64 size);
#define mem_zero(data, size) mem_set(data, 0, size)

typedef struct mem mem;
struct mem { uaddress address; u64 size; };

////////////////////////////////
// strings: u8 u16

typedef struct string8 string8;
struct string8 {
    u8* str;
    u64 size;
};

typedef struct string16 string16;
struct string16 {
    u16* str;
    u64 size;
};

b32 char_is_space(u8 c);
b32 char_is_upper(u8 c);
b32 char_is_lower(u8 c);
b32 char_is_alphabet(u8 c);
b32 char_is_digit(u8 c, u32 base);

u8 char_to_lower(u8 c);
u8 char_to_upper(u8 c);

u64 cstr8_length(u8 *c);
u64 cstr16_length(u16 *c);

string8 _str8(u8* str, u64 size);
#define str8(string_literal) _str8((u8*)(string_literal), sizeof(string_literal) - 1)
string16 _str16(u16* str, u64 size);
#define str16(string_literal) _str16((u16*)(string_literal), sizeof(string_literal) - 1)
string8  str8_from_cstr(char* str);
string16 str16_from_cstr(u16* str);

#if defined(MSTD_USE_ARENA)
typedef struct Arena Arena;
string8  str8_to_lower(Arena* arena, string8 str);
string8  str8_to_upper(Arena* arena, string8 str);
string16 str16_to_lower(Arena* arena, string16 str);
string16 str16_to_upper(Arena* arena, string16 str);
#endif

#endif // TYPES_IMPLEMENTATION