internal void* os_memory_reserve(U64 size, U64 large_pages) {
    void* result = (large_pages) ? VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE)
                                 : VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

internal void os_memory_commit(void* ptr, U64 size, U64 large_pages) {
    if(!large_pages)
        VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
}

internal void os_memory_decommit(void* ptr, U64 size) {
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

internal void os_memory_release(void* ptr, U64 size) {
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
}