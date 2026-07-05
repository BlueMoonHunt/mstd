global read_only U8 utf8_class[32] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 5,
};

internal UnicodeDecode utf8_decode(U8* str, U64 max) {
    UnicodeDecode result;
    U8 byte;
    U8 byte_class;

    result.inc = 1;
    result.codepoint = u32_max;

    byte = str[0];
    byte_class = utf8_class[byte >> 3];

    switch (byte_class) {
    case 1: {
        result.codepoint = byte;
    } break;
    case 2: {
        if (1 < max) {
            U8 cont_byte = str[1];
            if (utf8_class[cont_byte >> 3] == 0) {
                result.codepoint = (byte & 0x1f) << 6;
                result.codepoint |= (cont_byte & 0x3f);
                result.inc = 2;
            }
        }
    } break;
    case 3: {
        if (2 < max) {
            U8 cont_byte[2] = {str[1], str[2]};
            if (utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0) {
                result.codepoint = (byte & 0xf) << 12;
                result.codepoint |= ((cont_byte[0] & 0x3f) << 6);
                result.codepoint |= (cont_byte[1] & 0x3f);
                result.inc = 3;
            }
        }
    } break;
    case 4: {
        if (3 < max) {
            U8 cont_byte[3] = {str[1], str[2], str[3]};
            if (utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0 &&
                utf8_class[cont_byte[2] >> 3] == 0) {
                result.codepoint = (byte & 0x7) << 18;
                result.codepoint |= ((cont_byte[0] & 0x3f) << 12);
                result.codepoint |= ((cont_byte[1] & 0x3f) << 6);
                result.codepoint |= (cont_byte[2] & 0x3f);
                result.inc = 4;
            }
        }
    }
    }

    return result;
}

internal UnicodeDecode utf16_decode(U16* str, U64 max) {
    UnicodeDecode result;

    result.codepoint = str[0];
    result.inc = 1;

    if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000) {
        result.codepoint = ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
        result.inc = 2;
    }

    return result;
}

internal U32 utf8_encode(U8* str, U32 codepoint) {
    U32 inc = 0;

    if (codepoint <= 0x7F) {
        str[0] = (U8)codepoint;
        inc = 1;
    } else if (codepoint <= 0x7FF) {
        str[0] = (0x3 << 6) | ((codepoint >> 6) & 0x1f);
        str[1] = (1 << 7) | (codepoint & 0x3f);
        inc = 2;
    } else if (codepoint <= 0xFFFF) {
        str[0] = (0x7 << 5) | ((codepoint >> 12) & 0xf);
        str[1] = (1 << 7) | ((codepoint >> 6) & 0x3f);
        str[2] = (1 << 7) | (codepoint & 0x3f);
        inc = 3;
    } else if (codepoint <= 0x10FFFF) {
        str[0] = (0xf << 4) | ((codepoint >> 18) & 0x7);
        str[1] = (1 << 7) | ((codepoint >> 12) & 0x3f);
        str[2] = (1 << 7) | ((codepoint >> 6) & 0x3f);
        str[3] = (1 << 7) | (codepoint & 0x3f);
        inc = 4;
    } else {
        str[0] = '?';
        inc = 1;
    }

    return inc;
}

internal U32 utf16_encode(U16* str, U32 codepoint) {
    U32 inc = 1;

    if (codepoint == u32_max)
        str[0] = (U16)'?';
    else if (codepoint < 0x10000)
        str[0] = (U16)codepoint;
    else {
        U32 v = codepoint - 0x10000;
        str[0] = (U16)(0xD800 + (v >> 10));
        str[1] = (U16)(0xDC00 + (v & 0x3ff));
        inc = 2;
    }

    return inc;
}

internal U32 utf8_size(U32 cp) {
    if (cp <= 0x7F)
        return 1;
    if (cp <= 0x7FF)
        return 2;
    if (cp <= 0xFFFF)
        return 3;
    return 4;
}

internal U32 utf16_size(U32 cp) {
    if (cp > 0x10FFFF)
        return 0;
    if (cp >= 0xD800 && cp <= 0xDFFF)
        return 0;
    if (cp <= 0xFFFF)
        return 1;
    return 2;
}