internal VArray* varray_create_(U64 theoratical_capacity, U32 element_size, U32 element_alignment) {
    U64 reserve;
    U32 page_size;
    VArray* array;

    if (!element_alignment)
        element_alignment = 1;

    page_size   = mem_page_size();
    reserve     = align_up_pow2(align_up_pow2(sizeof(VArray), element_alignment) + theoratical_capacity * element_size, page_size);
    array       = mem_reserve(reserve, 0);

    mem_commit(array, page_size, 0);

    array->element_alignment    = element_alignment;
    array->element_size         = element_size;
    array->committed            = page_size;
    array->reserved             = reserve;
    array->size                 = 0;

    return array;
}

internal void varray_destroy(VArray* array) {
    mem_release(array, array->reserved);
}

internal void varray_resize(VArray* array, U64 new_capacity) {
    U64 target_committed;

    target_committed = varray_header_size(array) + new_capacity * array->element_size;
    target_committed = align_up_pow2(target_committed, mem_page_size());
    target_committed = clamp_top(target_committed, array->reserved);

    if (array->committed <= target_committed)
        mem_commit((U8*)array + array->committed, target_committed - array->committed, 0);
    else
        mem_decommit((U8*)array + target_committed, array->committed - target_committed);

    array->committed = target_committed;
}

internal void varray_push(VArray* array, void* elements, U64 count) {
    U8* items;
    U64 needed;

    needed = array->size + count;

    if (needed > varray_capacity(array))
        varray_resize(array, needed);

    items = (U8*)varray_get(array, array->size);

    if (items) {
        mem_copy(items, elements, count * array->element_size);
        array->size += count;
    }
}

internal void varray_pop(VArray* array, U64 count) {
    array->size = (count > array->size) ? 0 : array->size - count;
}

internal void varray_set(VArray* array, void* element, U64 index) {
    U8* item;

    item = (U8*)varray_get(array, index);

    if (item) {
        mem_copy(item, element, array->element_size);

        if (index >= array->size) {
            array->size = index + 1;
        }
    }
}

internal void* varray_get(VArray* array, U64 index) {
    U8* base;
    void* result;

    result = 0;

    if (index < varray_capacity(array)) {
        base   = (U8*)array + varray_header_size(array);
        result = base + index * array->element_size;
    }

    return result;
}

internal U64 varray_capacity(VArray* array) {
    return (array->committed - varray_header_size(array)) / array->element_size;
}

internal U64 varray_theoratical_capacity(VArray* array) {
    return (array->reserved - varray_header_size(array)) / array->element_size;
}

internal force_inline U64 varray_header_size(VArray* array) {
    return align_up_pow2(sizeof(VArray), array->element_alignment);
}