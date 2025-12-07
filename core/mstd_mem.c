#include "mstd_core.h"
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