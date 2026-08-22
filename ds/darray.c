internal force_inline void* darray_handle(Arena* arena, DArrayHeader* header, DArrayMetaData meta, U64 index) {
    U8** chunk_ptrs;
    I8 chunk_index;
    U64 chunk_capacity;
    U64 chunk_start_index;
    I8 i;

    chunk_ptrs = (U8**)(header + 1);
    chunk_index = (I8)u64_msb((index >> meta.shift) + 1);

    if (arena && chunk_index < (I8)meta.chunks_max) {
        for (i = 0; i <= chunk_index; i++) {
            if (chunk_ptrs[i] == 0) {
                chunk_capacity = 1ULL << clamp_top(i + meta.shift, 63);
                chunk_ptrs[i] = (U8*)arena_push(arena, chunk_capacity * meta.element_size, 8);
                header->size += chunk_capacity;
            }
        }
    }

    if (chunk_index > 0) {
        chunk_start_index = (1ULL << (chunk_index + meta.shift)) - (1ULL << meta.shift);
        index -= chunk_start_index;
    }

    return chunk_ptrs[chunk_index] + (index * meta.element_size);
}