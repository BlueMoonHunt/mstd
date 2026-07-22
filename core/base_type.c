#if COMPILER_MSVC

internal force_inline U8 u32_count_zerol(U32 x) {
    unsigned long zeros = 0;
    return _BitScanReverse(&zeros, x) ? (U8)(31 - zeros) : 32;
}
internal force_inline U8 u64_count_zerol(U64 x) {
    unsigned long zeros = 0;
    return _BitScanReverse64(&zeros, x) ? (U8)(63 - zeros) : 64;
}

internal force_inline U8 u32_count_zeror(U32 x) {
    unsigned long zeros = 0;
    _BitScanForward(&zeros, x);
    return (U8)zeros;
}
internal force_inline U8 u64_count_zeror(U64 x) {
    unsigned long zeros = 0;
    _BitScanForward64(&zeros, x);
    return (U8)zeros;
}

internal force_inline I8 u32_msb(U32 x) {
    unsigned long where;
    return _BitScanReverse(&where, x) ? (I8)where : -1;
}
internal force_inline I8 u64_msb(U64 x) {
    unsigned long where;
    return _BitScanReverse64(&where, x) ? (I8)where : -1;
}

internal force_inline I8 u32_lsb(U32 x) {
    unsigned long where;
    return _BitScanForward(&where, x) ? (I8)where : -1;
}
internal force_inline I8 u64_lsb(U64 x) {
    unsigned long where;
    return _BitScanForward64(&where, x) ? (I8)where : -1;
}

#endif