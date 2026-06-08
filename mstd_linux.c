#include <string.h>
#include <sys/mman.h>
#include <unistd.h>


////////////////////////////////
// Module: Memory

function void *mem_reserve(u64 size, u32 large_pages) {
    i32 flags = MAP_PRIVATE | MAP_ANONYMOUS;
    if (large_pages)
        flags |= MAP_HUGETLB;

    void *result = mmap(NULL, size, PROT_NONE, flags, -1, 0);
    return result;
}

function u8 mem_commit(void *ptr, u64 size, u32 large_pages) {
    u8 result = (large_pages) ? 1 : (mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0);
    return result;
}

function void mem_decommit(void *ptr, u64 size) { madvise(ptr, size, MADV_DONTNEED); }

function void mem_release(void *ptr, u64 size) { munmap(ptr, size); }

function u64 mem_page_size() { return sysconf(_SC_PAGESIZE); }

function u64 mem_large_page_size() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return 0;

    char buffer[256];
    long size = 0;

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strncmp(buffer, "Hugepagesize:", 13) == 0) {
            sscanf(buffer, "Hugepagesize: %ld", &size);
            size *= 1024;
            break;
        }
    }

    fclose(fp);
    return size;
}