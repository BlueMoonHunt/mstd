#if !defined(BASE_ARRAY_H)
#define BASE_ARRAY_H

/* VArray */

typedef struct VArray VArray;
struct VArray {
    U32 element_alignment;
    U32 element_size;
    U64 reserved;
    U64 committed;
    U64 size;
    U64 page_size;
};

internal VArray* varray_create_(U64 theoratical_capacity, U64 initial_size, U32 element_size, U32 element_alignment);
internal void varray_destroy(VArray* array);
internal void varray_resize(VArray* array, U64 new_capacity);
internal void varray_push(VArray* array, void* elements, U64 count);
internal void varray_pop(VArray* array, U64 count);
internal void varray_set(VArray* array, void* element, U64 index);
internal void* varray_get(VArray* array, U64 index);
internal U64 varray_capacity(VArray* array);

#define varray_create(theoratical_capacity, initial_size, T) varray_create_(theoratical_capacity, initial_size, sizeof(T), align_of(T))

#endif /* BASE_ARRAY_H */