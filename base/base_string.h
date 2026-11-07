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

typedef struct Str16 Str16;
typedef struct Str32 Str32;

typedef struct Str8 Str8;
struct Str8 {
    U8* data;
    U64 size;
};

enum {
    CHAR_DIGIT_MASK = mask(4)
};

typedef enum CharType {
    CHAR_TYPE_SPACE = (1 << 4),
    CHAR_TYPE_UPPER = (1 << 5),
    CHAR_TYPE_LOWER = (1 << 6),
    CHAR_TYPE_DIGIT = (1 << 7), /* NOTE: CHAR_TYPE_DIGIT can be removed.*/
} CharType;

global read_only align_to(64) U8 CHAR_LUT[256] = {
    // White-space: Tab, LF, VT, FF, CR, Space
    [0x09] = CHAR_TYPE_SPACE,
    [0x0A] = CHAR_TYPE_SPACE,
    [0x0B] = CHAR_TYPE_SPACE,
    [0x0C] = CHAR_TYPE_SPACE,
    [0x0D] = CHAR_TYPE_SPACE,
    [0x20] = CHAR_TYPE_SPACE,

    // Digits: 0-9 (Decimal + Hex)
    ['0'] = CHAR_TYPE_DIGIT | 0,
    ['1'] = CHAR_TYPE_DIGIT | 1,
    ['2'] = CHAR_TYPE_DIGIT | 2,
    ['3'] = CHAR_TYPE_DIGIT | 3,
    ['4'] = CHAR_TYPE_DIGIT | 4,
    ['5'] = CHAR_TYPE_DIGIT | 5,
    ['6'] = CHAR_TYPE_DIGIT | 6,
    ['7'] = CHAR_TYPE_DIGIT | 7,
    ['8'] = CHAR_TYPE_DIGIT | 8,
    ['9'] = CHAR_TYPE_DIGIT | 9,

    // Uppercase Hex: A-F
    ['A'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT | 10,
    ['B'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT | 11,
    ['C'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT | 12,
    ['D'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT | 13,
    ['E'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT | 14,
    ['F'] = CHAR_TYPE_UPPER | CHAR_TYPE_DIGIT | 15,

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
    ['a'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT | 10,
    ['b'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT | 11,
    ['c'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT | 12,
    ['d'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT | 13,
    ['e'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT | 14,
    ['f'] = CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT | 15,

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
#define char_is_space(c) (CHAR_LUT[(U8)(c)] & CHAR_TYPE_SPACE)
#define char_is_upper(c) (CHAR_LUT[(U8)(c)] & CHAR_TYPE_UPPER)
#define char_is_lower(c) (CHAR_LUT[(U8)(c)] & CHAR_TYPE_LOWER)
#define char_is_alpha(c) (CHAR_LUT[(U8)(c)] & (CHAR_TYPE_UPPER | CHAR_TYPE_LOWER))
#define char_is_alphanumeric(c) (CHAR_LUT[(U8)(c)] & (CHAR_TYPE_UPPER | CHAR_TYPE_LOWER | CHAR_TYPE_DIGIT))
#define char_is_numeric(c) (CHAR_LUT[(U8)(c)] & CHAR_TYPE_DIGIT)
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
internal Str8 str8_slice(Str8 str, U64 begin, U64 end);

/* String number convertor */

global read_only U8 NUM_CHAR_LUT[16] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
};

internal U32 str8_is_integer(Str8 str);

internal U64 u64_from_str8(Str8 str, U8 base);

internal Str8 str8_from_u64(Arena* arena, U64 num);

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