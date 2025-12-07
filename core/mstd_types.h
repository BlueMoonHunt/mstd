#ifndef TYPES_IMPLEMENTATION
#define TYPES_IMPLEMENTATION
#include <stdint.h>

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

#define KB(value) ((value) * 1024)
#define MB(value) ((value) * 1024 * KB(1))
#define GB(value) ((value) * 1024 * MB(1))
#define TB(value) ((value) * 1024 * GB(1))

////////////////////////////////
// STRING: utf8

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
b32 char_is_alpha(u8 c);
b32 char_is_slash(u8 c);
b32 char_is_digit(u8 c, u32 base);

u8 char_to_lower(u8 c);
u8 char_to_upper(u8 c);
u8 char_to_correct_slash(u8 c);

u64 cstr8_length(u8 *c);
u64 cstr16_length(u16 *c);

#endif // TYPES_IMPLEMENTATION