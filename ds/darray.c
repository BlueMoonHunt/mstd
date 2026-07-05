internal force_inline void* darray_handle(Arena* arena, DArrayHeader* header, DArrayMetaData meta, U64 index) {
    U8** chunks;
    U64 i_shift;
    I8 chunks_i;
    I8 i;
    U64 base;

    chunks = (U8**)(header + 1);
    i_shift = index >> meta.shift;
    chunks_i = u64_msb(i_shift + 1);

    if (arena && chunks_i < meta.chunks_max) {
        for (i = 0; i <= chunks_i; ++i) {
            if (chunks[i] == 0) {
                U64 tier_size = 1ULL << clamp_top((i + meta.shift), 63);
                chunks[i] = (U8*)arena_push(arena, tier_size * meta.el_size, 8);
            }
        }
    }

    if (chunks_i > 0) {
        base = (1ULL << (chunks_i + meta.shift)) - (1ULL << meta.shift);
        index -= base;
    }

    return chunks[chunks_i] + (index * meta.el_size);
}