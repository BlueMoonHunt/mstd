/* Unicode */
typedef struct UnicodeDecode {
    U32 inc;
    U32 codepoint;
} UnicodeDecode;

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

global read_only align_to(64) U8 ASCII_LUT[256] = {
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
internal Str8 str8_from_str16(Arena* arena, Str16 str);
internal Str8 str8_from_str32(Arena* arena, Str32 str);
internal Str8 str8_from_size(Arena* arena, U64 size);
internal Str8 str8_from_slice(Arena* arena, Str8Slice slice);

/* String Matching */

typedef U32 Str8MatchFlag;

enum {
    STR8_MATCH_FLAG_NONE                = 0,
    STR8_MATCH_FLAG_CASE_INSENSITIVE    = bit(0),
    STR8_MATCH_FLAG_SLASH_INSENSITIVE   = bit(1),
    STR8_MATCH_FLAG_COUNT,
};

internal U32 str8_match(Str8 a, Str8 b, Str8MatchFlag flags);
internal U64 str8_find(Str8 target, Str8 query, U64 offset, Str8MatchFlag flags);
internal U64 str8_find_reverse(Str8 target, Str8 query, U64 offset, Str8MatchFlag flags);

/* String concatinate and copy */
internal Str8 str8_concat(Arena* arena, Str8 a, Str8 b);
internal Str8 str8_concat_n_(Arena* arena, ...);

#define str8_concat_n(arena, ...) str8_concat_n_(arena, __VA_ARGS__, (Str8){0})

internal Str8 str8_copy(Arena* arena, Str8 str);
internal char* str8_copy_to_cstr(Arena* arena, Str8 str);

/* String slicing */
internal Str8Slice str8_slice(Str8Slice str, U64 begin, U64 end);

#define str8_slice_from_str8(str, begin, end) str8_slice((Str8Slice)str, begin, end)
#define str8_slice_match(a, b, flags) str8_match((Str8)a, (Str8)b, flags)
#define str8_slice_find(target, query, offset, flags) str8_find((Str8)target, (Str8)query, offset, flags)
#define str8_slice_find_reverse(target, query, offset, flags) str8_find_reverse((Str8)target, (Str8)query, offset, flags)

typedef struct Str16 {
    U16* data;
    U64 size;
} Str16;

/* String Constructors */
#define str16_lit(S) ((Str16){ .size = (sizeof(S) / sizeof((*S))) - 1, .data = (S) })
#define str16(S) str16_from_cstr(S)

internal Str16 str16_from_cstr(U16* str);
internal Str16 str16_from_str8(Arena* arena, Str8 str);
internal Str16 str16_from_size(Arena* arena, U64 size);

typedef struct Str32 {
    U32* data;
    U64 size;
} Str32;

/* String Constructors */
#define str32_lit(S) ((Str32){ .size = (sizeof(S) / sizeof((*S))) - 1, .data = (S) })
#define str32(S) str32_from_cstr(S)

internal Str32 str32_from_cstr(U32* str);
internal Str32 str32_from_str8(Arena* arena, Str8 str);
internal Str32 str32_from_size(Arena* arena, U64 size);