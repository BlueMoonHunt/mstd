#include "mstd.h"
#define NOMINMAX
#include <Windows.h>
#include <bcrypt.h>

#pragma comment(lib, "bcrypt")

SystemInfo* os_get_system_info(Arena* arena) {
    SystemInfo* s_info = arena_push_struct(arena, SystemInfo);
    if (!s_info) return NULL;

    SYSTEM_INFO sysinfo = { 0 };
    GetSystemInfo(&sysinfo);

    s_info->page_size = sysinfo.dwPageSize;
    s_info->large_page_size = GetLargePageMinimum();
    s_info->allocation_granularity = sysinfo.dwAllocationGranularity;
    s_info->processors = sysinfo.dwNumberOfProcessors;
    s_info->numa_nodes = 1; // Default
    s_info->cache_line_size = 64; // Safe default (x86/x64 standard)

    DWORD buffer_size = 0;
    GetLogicalProcessorInformationEx(RelationAll, NULL, &buffer_size);
    if (buffer_size > 0) arena_scratch(arena) {
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer =
            (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)arena_push(arena, buffer_size, alignof(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX));

        if (buffer && GetLogicalProcessorInformationEx(RelationAll, buffer, &buffer_size)) {
            u32 numa_count = 0;
            u64 max_cache_line = 0;

            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX current = buffer;
            DWORD bytes_processed = 0;

            while (bytes_processed < buffer_size) {
                switch (current->Relationship) {
                case RelationNumaNode:
                    numa_count++;
                    break;
                case RelationCache:
                    if (current->Cache.Type == CacheData || current->Cache.Type == CacheUnified) {
                        if (current->Cache.LineSize > max_cache_line) {
                            max_cache_line = current->Cache.LineSize;
                        }
                    }
                    break;
                }
                bytes_processed += current->Size;
                current = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((u8*)current + current->Size);
            }

            s_info->numa_nodes = numa_count > 0 ? numa_count : 1;
            s_info->cache_line_size = max_cache_line > 0 ? max_cache_line : 64;
        }
    }

    return s_info;
}

ProcessInfo* os_get_process_info(Arena* arena) {
    ProcessInfo* p_info = arena_push_struct(arena, ProcessInfo);
    if (!p_info) return NULL;

    p_info->pid = GetCurrentProcessId();
    p_info->group_id = 0;

    HANDLE h_proc = GetCurrentProcess();

    DWORD_PTR process_mask, system_mask;
    if (GetProcessAffinityMask(h_proc, &process_mask, &system_mask))
        p_info->affinity_mask = (u64)process_mask;
    else
        p_info->affinity_mask = (u64)system_mask;

    SIZE_T min_ws, max_ws;
    DWORD flags;
    if (GetProcessWorkingSetSizeEx(h_proc, &min_ws, &max_ws, &flags))
        p_info->page_file_limit = (u64)max_ws;
    else
        p_info->page_file_limit = (u64)-1;

    return p_info;
}

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
    b32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != NULL);
    return result;
}

b32 os_commit_large(void* ptr, u64 size) {
    unreferenced_parameter(size); // because windows
    unreferenced_parameter(ptr); // because windows
    return 1;
}

void os_decommit(void* ptr, u64 size) {
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

void os_release(void* ptr, u64 size) {
    unreferenced_parameter(size); // because windows
    VirtualFree(ptr, 0, MEM_RELEASE);
}

b64 os_get_random_bits() {
    b64 val = 0;
    BCryptGenRandom(NULL, (PUCHAR)&val, sizeof(val), 0);
    return val;
}

#if COMPILER_MSVC
    #include "mstd_msvc.c"
#endif