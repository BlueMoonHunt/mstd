#ifndef MSTD_MEM_IMPLEMENTATION
#define MSTD_MEM_IMPLEMENTATION
#include "mstd_types.h"

b32 mem_is_pow2(uaddress address);
b32 mem_is_aligned(void* data, u64 alignment);
b32 mem_compare(void* data1, void* data2, u64 count);
void mem_copy(void* destination, void* source, u64 size);
void mem_set(void* data, u8 value, u64 size);
#define mem_zero(data, size) mem_set(data, 0, size)
inline static void* mem_aligned_alloc(u64 size, u64 alignment);
inline static void mem_aligned_free(void* ptr);

#endif // MSTD_MEM_IMPLEMENTATION