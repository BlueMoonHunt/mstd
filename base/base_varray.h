#if !defined(BASE_ARRAY_H)
#define BASE_ARRAY_H

/* VArray */

typedef struct VArray VArray;
struct VArray {
    U32 element_size;
    U32 element_alignment;
    U64 size;
    U64 reserved;
    U64 committed;
};

internal VArray* varray_create_(U64 theoratical_capacity, U32 element_size, U32 element_alignment);

internal void varray_destroy(VArray* array);

internal void varray_resize(VArray* array, U64 new_capacity);

internal void varray_push(VArray* array, void* elements, U64 count);
internal void varray_pop(VArray* array, U64 count);

internal void varray_set(VArray* array, void* element, U64 index);
internal void* varray_get(VArray* array, U64 index);

internal force_inline U64 varray_capacity(VArray* array);
internal force_inline U64 varray_theoratical_capacity(VArray* array);
internal force_inline U64 varray_header_size(VArray* array);

#define varray_create(theoratical_capacity, T)  varray_create_((theoratical_capacity), sizeof(T), align_of(T))
#define varray_set_val(array, T, value, index)  varray_set((array), &(T){(value)}, (index))
#define varray_push_val(array, T, value)        varray_push((array), &(T){(value)}, 1)

#endif /* BASE_ARRAY_H */