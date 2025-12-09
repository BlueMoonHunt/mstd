#include "mstd.h"
#define NOMINMAX
#include <Windows.h>

u64 os_get_page_size() {
    SYSTEM_INFO sysinfo = { 0 };
    GetSystemInfo(&sysinfo);
    return sysinfo.dwPageSize;
}

u64 os_get_large_page_size() {
    return GetLargePageMinimum();
}

void* os_reserve(u64 size) {
    void* result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

void* os_reserve_large(u64 size) {
    void* result = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
    return result;
}

b32 os_commit(void* ptr, u64 size) {
    b32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
    return result;
}

b32 os_commit_large(void* ptr, u64 size) {
    return 1;
}

void os_decommit(void* ptr, u64 size) {
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

void os_release(void* ptr, u64 size) {
    VirtualFree(ptr, 0, MEM_RELEASE);
}