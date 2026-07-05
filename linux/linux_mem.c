#include <sys/mman.h>
#include <unistd.h>

internal void* mem_reserve(U64 size, U32 large_pages) {
    I32 flags;
    void* result;

    flags = MAP_PRIVATE | MAP_ANONYMOUS;
    if (large_pages)
        flags |= MAP_HUGETLB;

    result = mmap(NULL, size, PROT_NONE, flags, -1, 0);

    return result;
}

internal U8 mem_commit(void* ptr, U64 size, U32 large_pages) {
    U8 result;

    result = (large_pages) ? 1 : (mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0);

    return result;
}

internal void mem_decommit(void *ptr, U64 size) {
    madvise(ptr, size, MADV_DONTNEED);
}

internal void mem_release(void *ptr, U64 size) {
    munmap(ptr, size);
}

internal U64 mem_page_size() {
    return sysconf(_SC_PAGESIZE);
}

internal U64 mem_large_page_size() {
    FILE *fp;
    char buffer[256];
    long size;

    fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return 0;

    for (size = 0; fgets(buffer, sizeof(buffer), fp);) {
        if (mem_match(buffer, "Hugepagesize:", 13)) {
            sscanf(buffer, "Hugepagesize: %ld", &size);
            size *= 1024;
            break;
        }
    }

    fclose(fp);
    return size;
}