#ifndef MSTD_OS_IMPLEMENTATION
#define MSTD_OS_IMPLEMENTATION
#include <core/mstd_core.h>

typedef struct OSInfo OSInfo;
struct OSInfo {
    u32 logical_processor_count;
    u64 page_size;
    u64 large_page_size;
    u64 allocation_granularity;
    string8 machine_name;
};

u64 os_get_page_size();
u64 os_get_large_page_size();
void* os_reserve(u64 size);
void* os_reserve_large(u64 size);
b32 os_commit(void* ptr, u64 size);
b32 os_commit_large(void* ptr, u64 size);
void os_decommit(void* ptr, u64 size);
void os_release(void* ptr, u64 size);

#endif // MSTD_OS_IMPLEMENTATION