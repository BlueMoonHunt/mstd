#include "mstd_mem.h"
#include <string.h>

b32 mem_is_pow2(uaddress address) {
    return is_pow2(address);
}

b32 mem_is_aligned(void* memory, u64 alignment) {
    return align_up_pad_pow2((uintptr_t)memory, alignment) == 0;
}

b32 mem_compare(void* memory1, void* memory2, u64 count) {
    return memcmp(memory1, memory2, count) == 0;
}

void mem_copy(void* destination, void* source, u64 size) {
    memcpy(destination, source, size);
}

void mem_set(void* memory, u8 value, u64 size) {
    memset(memory, value, size);
}

inline void* mem_aligned_alloc(u64 size, u64 alignment) {
#if defined(_MSC_VER) // MSVC
    return _aligned_malloc(size, alignment);
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    // use C11 aligned_alloc if available
    size_t padded = (size + alignment - 1) & ~(alignment - 1);
    return aligned_alloc(alignment, padded);
#elif defined(_POSIX_VERSION)
    void* ptr = NULL;
    if (posix_memalign(&ptr, alignment, size))
        return NULL;
    return ptr;
#else
#error "No aligned allocation available"
#endif
}

inline void mem_aligned_free(void* ptr) {
#if defined(_MSC_VER)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}