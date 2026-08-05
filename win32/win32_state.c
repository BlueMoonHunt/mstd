#pragma comment(lib, "user32")
#pragma comment(lib, "shell32")
#pragma comment(lib, "advapi32")
#pragma comment(lib, "synchronization")

internal SystemInfo* os_get_system_info(void) {
    return &os_state.system_info;
}

internal ProcessInfo* os_get_process_info(void) {
    return &os_state.process_info;
}

internal Str8 os_get_current_working_directory(Arena* arena) {
    Str8 path = {0};

    Arena* scratch = arena_scratch_begin();

    DWORD length = GetCurrentDirectoryW(0, 0);
    Str16 path16 = {0};
    path16.data = arena_push_array(scratch, U16, length);
    path16.size = GetCurrentDirectoryW(length, (WCHAR*)path16.data);
    path = str8_from_16(arena, path16);

    arena_scratch_end(scratch);

    return path;
}

internal U64 os_ticks_now(void) {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (U64)(counter.QuadPart);
}

internal void os_state_init(void) {
    SYSTEM_INFO system_info = {0};
    LARGE_INTEGER resolution;
    Arena* scratch;
    Str8 bin_path;
    U64 buffer_size;
    U16* path;
    U64 path_length;

    GetSystemInfo(&system_info);
    os_state.system_info.logical_processor_count     = (U64)system_info.dwNumberOfProcessors;
    os_state.system_info.page_size                   = system_info.dwPageSize;
    os_state.system_info.allocation_granularity      = system_info.dwAllocationGranularity;
    os_state.system_info.large_page_size             = GetLargePageMinimum();

    os_state.system_info.microsecond_resolution = 1;
    if (QueryPerformanceFrequency(&resolution))
        os_state.system_info.microsecond_resolution = resolution.QuadPart;

    os_state.arena = arena_alloc_default(); /* requires [os_state.system_info.page_size] */

    scratch = arena_scratch_begin();
    buffer_size = KB(32);
    path = arena_push_array(scratch, U16, buffer_size);
    path_length = GetModuleFileNameW(0, path, buffer_size);
    bin_path = str8_from_16(scratch, (Str16){.size = path_length, .data = path});

    os_state.process_info.bin_path                   = str8_copy(os_state.arena, bin_path);
    os_state.process_info.id                         = GetCurrentProcessId();
    os_state.process_info.current_working_directory  = os_get_current_working_directory(os_state.arena);

    arena_scratch_end(scratch);
}