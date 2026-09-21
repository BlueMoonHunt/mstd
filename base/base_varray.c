internal VArray* varray_create(U64 theoratical_capacity, U64 initial_size, U32 element_size, U32 element_alignment) {
    U64 page_size;
    U64 reserve;
    U64 commit;
    U64 base_size;
    VArray* array;

    if (!element_alignment)
        element_alignment = 1;

    page_size = mem_page_size();
    base_size = align_up_pow2(sizeof(VArray), element_alignment);
    reserve = theoratical_capacity  ? align_up_pow2(base_size + theoratical_capacity * element_size, page_size) : MB(100);
    commit = initial_size ? align_up_pow2(base_size + initial_size * element_size, page_size) : page_size;

    array = mem_reserve(reserve, 0);

    mem_commit(array, commit, 0);

    array->element_size         = element_size;
    array->element_alignment    = element_alignment;
    array->committed            = commit;
    array->reserved             = reserve;
    array->page_size            = page_size;

    return array;
}

internal void varray_destroy(VArray* array) {
    mem_release(array, array->reserved);
}

internal void varray_resize(VArray* array, U64 new_capacity) {
    U64 base_size;
    U64 target_committed;

    base_size = align_up_pow2(sizeof(VArray), array->element_alignment);
    target_committed = align_up_pow2(base_size + new_capacity * array->element_size, array->page_size);

    if (target_committed > array->reserved)
        target_committed = array->reserved;

    if (array->committed < target_committed)
        mem_commit((U8*)array + array->committed, target_committed - array->committed, 0);
    else if (array->committed > target_committed)
        mem_decommit((U8*)array + target_committed, array->committed - target_committed);

    array->committed = target_committed;
}

internal void varray_push(VArray* array, void* elements, U64 count) {
    U8* base;
    U64 capacity;
    U64 needed;

    capacity = varray_capacity(array);
    needed = array->size + count;

    if (needed > capacity)
        varray_resize(array, needed);

    base = (U8*)array + align_up_pow2(sizeof(VArray), array->element_alignment);
    mem_copy(base + array->size * array->element_size, elements, count * array->element_size);

    array->size += count;
}

internal void varray_pop(VArray* array, U64 count) {
    array->size = (count >= array->size) ? 0 : array->size - count;
}

internal void varray_set(VArray* array, void* element, U64 index) {
    U8* base;

    if (index >= varray_capacity(array))
        varray_resize(array, index + 1);

    base = (U8*)array + align_up_pow2(sizeof(VArray), array->element_alignment);
    mem_copy(base + index * array->element_size, element, array->element_size);

    if (index >= array->size)
        array->size = index + 1;
}

internal void* varray_get(VArray* array, U64 index) {
    U8* base;
    void* result;

    result = 0;

    if (index < array->size) {
        base = (U8*)array + align_up_pow2(sizeof(VArray), array->element_alignment);
        result = base + index * array->element_size;
    }

    return result;
}

internal U64 varray_capacity(VArray* array) {
    U64 base_size;

    base_size = align_up_pow2(sizeof(VArray), array->element_alignment);

    return (array->committed > base_size) ? (array->committed - base_size) / array->element_size : 0;
}