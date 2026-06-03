#include "mstd.h"
#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "advapi32")
#pragma comment(lib, "synchronization")

////////////////////////////////
// Module: Memory

function void *mem_reserve(u64 size, u32 large_pages) {
    void *result =
        (large_pages)
            ? VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES,
                           PAGE_READWRITE)
            : VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

function u8 mem_commit(void *ptr, u64 size, u32 large_pages) {
    u8 result =
        (large_pages)
            ? 1
            : (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != NULL);
    return result;
}

function void mem_decommit(void *ptr, u64 size) {
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

function void mem_release(void *ptr, u64 size) {
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
}

function u64 mem_page_size(void) {
    SYSTEM_INFO sysinfo = {0};
    GetSystemInfo(&sysinfo);
    return sysinfo.dwPageSize;
}

function u64 mem_large_page_size(void) { return GetLargePageMinimum(); }

////////////////////////////////
// CLI

function void cli_attach_if_exists(void) {
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        HANDLE hOut =
            CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        HANDLE hIn = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

        debug_assert(hOut != INVALID_HANDLE_VALUE);
        debug_assert(hIn != INVALID_HANDLE_VALUE);

        SetStdHandle(STD_OUTPUT_HANDLE, hOut);
        SetStdHandle(STD_ERROR_HANDLE, hOut);
        SetStdHandle(STD_INPUT_HANDLE, hIn);

        FILE *dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
        freopen_s(&dummy, "CONIN$", "r", stdin);

        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, mode);
        }
    }
}

////////////////////////////////
// Module: Thread

typedef struct Win32ThreadCtx {
    HANDLE handle;
    DWORD id;
    ThreadEntryPointFn *user_func;
    void *user_data;
} Win32ThreadCtx;

internal DWORD WINAPI Win32_ThreadTrampoline(LPVOID param) {
    Win32ThreadCtx *ctx = (Win32ThreadCtx *)param;
    if (ctx && ctx->user_func) {
        ctx->user_func(ctx->user_data);
    }
    return 0;
}

function void thread_attach(Thread *thread, ThreadEntryPointFn *func,
                            void *data) {
    Win32ThreadCtx *ctx = (Win32ThreadCtx *)&thread->reserved;
    ctx->user_func = func;
    ctx->user_data = data;
    ctx->handle =
        CreateThread(NULL, 0, Win32_ThreadTrampoline, ctx, 0, &ctx->id);
}

function u32 thread_join(Thread *thread) {
    Win32ThreadCtx *ctx = (Win32ThreadCtx *)thread->reserved;
    DWORD result = WaitForSingleObject(ctx->handle, INFINITE);
    CloseHandle(ctx->handle);
    return (result == WAIT_OBJECT_0);
}

function void thread_detach(Thread *thread) {
    Win32ThreadCtx *ctx = (Win32ThreadCtx *)thread->reserved;
    CloseHandle(ctx->handle);
}

function u32 thread_id(void) { return (u32)GetCurrentThreadId(); }

function void thread_sleep(u32 ms) { Sleep((DWORD)ms); }

////////////////////////////////
// Mutex

function void mutex_init(Mutex *mutex) {
    InitializeCriticalSection((LPCRITICAL_SECTION)mutex->reserved);
}

function void mutex_take(Mutex *mutex) {
    EnterCriticalSection((LPCRITICAL_SECTION)mutex->reserved);
}

function void mutex_drop(Mutex *mutex) {
    LeaveCriticalSection((LPCRITICAL_SECTION)mutex->reserved);
}

function void mutex_destroy(Mutex *mutex) {
    DeleteCriticalSection((LPCRITICAL_SECTION)mutex->reserved);
}

////////////////////////////////
// Read/Write Mutex

function void rw_mutex_init(RWMutex *mutex) {
    InitializeSRWLock((PSRWLOCK)mutex->reserved);
}

function void rw_mutex_take(RWMutex *mutex, u32 write_mode) {
    if (write_mode) {
        AcquireSRWLockExclusive((PSRWLOCK)mutex->reserved);
    } else {
        AcquireSRWLockShared((PSRWLOCK)mutex->reserved);
    }
}

function void rw_mutex_drop(RWMutex *mutex, u32 write_mode) {
    if (write_mode) {
        ReleaseSRWLockExclusive((PSRWLOCK)mutex->reserved);
    } else {
        ReleaseSRWLockShared((PSRWLOCK)mutex->reserved);
    }
}

// SRWLOCK does not require explicit destruction on Windows.
function void rw_mutex_destroy(RWMutex *mutex) { (void)mutex; }

////////////////////////////////
// Condition Variable

function void cond_var_init(CondVar *var) {
    InitializeConditionVariable((PCONDITION_VARIABLE)var->reserved);
}

function u32 cond_var_wait(CondVar *var, Mutex *mutex) {
    BOOL result =
        SleepConditionVariableCS((PCONDITION_VARIABLE)var->reserved,
                                 (PCRITICAL_SECTION)mutex->reserved, INFINITE);
    return (u32)result;
}

function u32 cond_var_wait_rw(CondVar *var, RWMutex *mutex, u32 write_mode) {
    ULONG flags = write_mode ? 0 : CONDITION_VARIABLE_LOCKMODE_SHARED;
    BOOL result =
        SleepConditionVariableSRW((PCONDITION_VARIABLE)var->reserved,
                                  (PSRWLOCK)mutex->reserved, INFINITE, flags);
    return (u32)result;
}

function void cond_var_signal(CondVar *var) {
    WakeConditionVariable((PCONDITION_VARIABLE)var->reserved);
}

function void cond_var_broadcast(CondVar *var) {
    WakeAllConditionVariable((PCONDITION_VARIABLE)var->reserved);
}

// CONDITION_VARIABLE does not require explicit destruction on Windows.
function void cond_var_destroy(CondVar *var) { (void)var; }

////////////////////////////////
// Semaphore

function void semaphore_init(Semaphore *semaphore, u32 initial_count,
                             u32 max_count) {
    HANDLE handle =
        CreateSemaphoreW(NULL, (LONG)initial_count, (LONG)max_count, NULL);
    *((HANDLE *)semaphore->reserved) = handle;
}

function u32 semaphore_take(Semaphore *semaphore) {
    HANDLE handle = *((HANDLE *)semaphore->reserved);
    DWORD result = WaitForSingleObject(handle, INFINITE);
    return (result == WAIT_OBJECT_0);
}

function void semaphore_drop(Semaphore *semaphore) {
    HANDLE handle = *((HANDLE *)semaphore->reserved);
    ReleaseSemaphore(handle, 1, NULL);
}

function void semaphore_destroy(Semaphore *semaphore) {
    HANDLE handle = *((HANDLE *)semaphore->reserved);
    CloseHandle(handle);
}

////////////////////////////////
// Barriers

function void barrier_init(Barrier *barrier, u32 count) {
    InitializeSynchronizationBarrier(
        (LPSYNCHRONIZATION_BARRIER)barrier->reserved, (LONG)count, -1);
}

function void barrier_wait(Barrier *barrier) {
    EnterSynchronizationBarrier((LPSYNCHRONIZATION_BARRIER)barrier->reserved,
                                0);
}

function void barrier_destroy(Barrier *barrier) {
    DeleteSynchronizationBarrier((LPSYNCHRONIZATION_BARRIER)barrier->reserved);
}

////////////////////////////////
// Hardware Atomics wait-on-address

function void atomic_wait_u32(atomic_u32 *addr, u32 expected_value) {
    WaitOnAddress((volatile PVOID)addr, (PVOID)&expected_value, sizeof(u32),
                  INFINITE);
}

function void atomic_wake_single(atomic_u32 *addr) {
    WakeByAddressSingle((PVOID)addr);
}

function void atomic_wake_all(atomic_u32 *addr) {
    WakeByAddressAll((PVOID)addr);
}

////////////////////////////////
// Module: File

function u32 file_delete(Str8 path) {
    int max_retries = 5;
    int sleep_ms = 1;

    for (int i = 0; i < max_retries; i++) {
        if (DeleteFileA((char *)path.data))
            return 1;

        DWORD err = GetLastError();
        if (err == ERROR_SHARING_VIOLATION || err == ERROR_LOCK_VIOLATION) {
            Sleep(sleep_ms);
            sleep_ms *= 2;
        } else
            break;
    }
    return 0;
}

function u32 file_copy(Str8 src, Str8 dest) {
    int max_retries = 5;
    int sleep_ms = 1;

    for (int i = 0; i < max_retries; i++) {
        if (CopyFileExA((char *)src.data, (char *)dest.data, NULL, NULL, FALSE,
                        COPY_FILE_NO_BUFFERING))
            return 1;

        DWORD err = GetLastError();
        if (err == ERROR_SHARING_VIOLATION || err == ERROR_LOCK_VIOLATION) {
            Sleep(sleep_ms);
            sleep_ms *= 2;
        } else
            break;
    }

    return 0;
}

function u32 file_move(Str8 src, Str8 dest) {
    int max_retries = 5;
    int sleep_ms = 1;

    for (int i = 0; i < max_retries; i++) {
        if (MoveFileExA((char *)src.data, (char *)dest.data,
                        MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED |
                            MOVEFILE_WRITE_THROUGH)) {
            return 1;
        }

        DWORD err = GetLastError();
        if (err == ERROR_SHARING_VIOLATION || err == ERROR_LOCK_VIOLATION) {
            Sleep(sleep_ms);
            sleep_ms *= 2;
        } else
            break;
    }
    return 0;
}

function u32 file_exists(Str8 path) {
    DWORD attributes = GetFileAttributesA((char *)path.data);
    u32 exists = (attributes != INVALID_FILE_ATTRIBUTES) &&
                 !!(~attributes & FILE_ATTRIBUTE_DIRECTORY);
    return exists;
}

function u32 file_directory_exists(Str8 path) {
    DWORD attributes = GetFileAttributesA((char *)path.data);
    u32 exists = (attributes != INVALID_FILE_ATTRIBUTES) &&
                 (attributes & FILE_ATTRIBUTE_DIRECTORY);
    return exists;
}

function FileHandle file_open(Str8 name, FileAccessFlag flags) {
    FileHandle result = {0};
    Arena *arena = arena_scratch_alloc();
    Str16 path_w32 = str16_from_8(arena, name);
    DWORD access_flags = 0;
    DWORD share_mode = 0;
    DWORD creation_disposition = OPEN_EXISTING;

    if (flags & FILE_ACCESS_FLAG_READ) {
        access_flags |= GENERIC_READ;
    }
    if (flags & FILE_ACCESS_FLAG_WRITE) {
        access_flags |= GENERIC_WRITE;
        creation_disposition = CREATE_ALWAYS;
    }
    if (flags & FILE_ACCESS_FLAG_EXECUTE) {
        access_flags |= GENERIC_EXECUTE;
    }
    if (flags & FILE_ACCESS_FLAG_SHARE_READ) {
        share_mode |= FILE_SHARE_READ;
    }
    if (flags & FILE_ACCESS_FLAG_SHARE_WRITE) {
        share_mode |= FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    }
    if (flags & FILE_ACCESS_FLAG_APPEND) {
        access_flags &= ~GENERIC_WRITE;
        access_flags |= FILE_APPEND_DATA;
        creation_disposition = OPEN_ALWAYS;
    }

    HANDLE file =
        CreateFileW((WCHAR *)path_w32.data, access_flags, share_mode, 0,
                    creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);
    if (file != INVALID_HANDLE_VALUE)
        result.val[0] = (u64)file;
    arena_scratch_release(arena);
    debug_assert(result.val[0]);

    return result;
}

function u64 file_size(FileHandle handle) {
    u64 size = 0;
    GetFileSizeEx((HANDLE)handle.val[0], (LARGE_INTEGER *)&size);
    return size;
}

function void file_close(FileHandle handle) {
    CloseHandle((HANDLE)handle.val[0]);
}

function u8 *file_read_ex(Arena *arena, FileHandle handle, u64 offset,
                          u64 size) {
    u64 total_read_size = 0;

    if (handle.val[0]) {
        HANDLE file = (HANDLE)handle.val[0];
        u64 _size = 0;
        GetFileSizeEx(file, (LARGE_INTEGER *)&_size);

        u64 total_to_read = clamp_top(size, _size);
        u8 *out = arena_push_array(arena, u8, total_to_read);

        for (u64 _offset = offset; total_read_size < total_to_read;) {
            u64 remaining = total_to_read - total_read_size;
            DWORD to_read = (DWORD)clamp_top(remaining, u32_max);
            DWORD actual_read = 0;

            OVERLAPPED overlapped = {0};
            overlapped.Offset = (DWORD)(_offset & u32_max);
            overlapped.OffsetHigh = (DWORD)(_offset >> 32);

            if (ReadFile(file, (u8 *)out + total_read_size, to_read,
                         &actual_read, &overlapped)) {
                _offset += actual_read;
                total_read_size += actual_read;
                if (actual_read < to_read)
                    break;
            } else
                break;
        }
        return out;
    }

    return 0;
}

function void file_write_ex(FileHandle handle, void *data, u64 offset,
                            u64 size) {
    u64 total_written = 0;

    if (handle.val[0]) {
        for (u64 dest_offset = offset;;) {

            u64 remaining = size - total_written;
            if (remaining == 0)
                break;

            DWORD to_write = (DWORD)clamp_top(remaining, MB(1));
            DWORD actual_write = 0;

            OVERLAPPED overlapped = {0};
            overlapped.Offset = (DWORD)(dest_offset & 0xFFFFFFFF);
            overlapped.OffsetHigh = (DWORD)(dest_offset >> 32);

            if (!WriteFile((HANDLE)handle.val[0], (u8 *)data + total_written,
                           to_write, &actual_write, &overlapped))
                break;
            total_written += actual_write;
            dest_offset += actual_write;

            if (remaining == 0)
                break;
        }
    }
}

////////////////////////////////
// Module: File Watcher

struct Win32FileWatcher {
    HANDLE dir_handle;
    HANDLE iocp;
    u8 notification_buffer[KB(8)];
    OVERLAPPED overlapped;
    u32 scan_sub_directories;
};

function FileWatcher file_watcher_create(Str8 path, u32 watch_sub_directory) {
    Arena *arena = arena_scratch_alloc();
    Str16 u16_path = str16_from_8(arena, path);

    HANDLE dir = CreateFileW(
        (LPCWSTR)u16_path.data, FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

    struct Win32FileWatcher watcher = {0};

    if (dir == INVALID_HANDLE_VALUE)
        return (watcher);

    watcher.scan_sub_directories = watch_sub_directory;
    watcher.dir_handle = dir;
    watcher.iocp = CreateIoCompletionPort(watcher.dir_handle, NULL, 0, 1);

    ReadDirectoryChangesW(
        watcher.dir_handle, watcher.notification_buffer,
        sizeof(watcher.notification_buffer), watcher.scan_sub_directories,
        FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME, NULL,
        &watcher.overlapped, NULL);

    arena_scratch_release(arena);
    return (watcher);
}

function FileEvent *file_watcher_poll_events(FileWatcher *watcher, Arena *arena,
                                             u32 timeout_ms, u32 *out_count) {
    DWORD bytes = 0;
    ULONG_PTR key = 0;
    LPOVERLAPPED p_overlapped = NULL;

    FileEvent *result_array = NULL;
    u32 count = 0;

    if (GetQueuedCompletionStatus(watcher->iocp, &bytes, &key, &p_overlapped,
                                  (DWORD)timeout_ms)) {
        if (p_overlapped == &watcher->overlapped && bytes > 0) {

            FILE_NOTIFY_INFORMATION *notify =
                (FILE_NOTIFY_INFORMATION *)watcher->notification_buffer;
            for (;;) {
                count++;
                if (notify->NextEntryOffset == 0)
                    break;
                notify = (FILE_NOTIFY_INFORMATION *)((u8 *)notify +
                                                     notify->NextEntryOffset);
            }

            result_array = arena_push_array(arena, FileEvent, count);

            notify = (FILE_NOTIFY_INFORMATION *)watcher->notification_buffer;

            for (u32 i = 0; i < count; i++) {
                u32 name_len_chars = notify->FileNameLength / sizeof(u16);
                Str16 u16_file = {(u16 *)notify->FileName, name_len_chars};

                result_array[i].file_name = str8_from_16(arena, u16_file);
                switch (notify->Action) {
                case FILE_ACTION_ADDED:
                case FILE_ACTION_RENAMED_NEW_NAME:
                    result_array[i].type = FILE_EVENT_TYPE_ADDED;
                    break;

                case FILE_ACTION_REMOVED:
                case FILE_ACTION_RENAMED_OLD_NAME:
                    result_array[i].type = FILE_EVENT_TYPE_DELETED;
                    break;

                case FILE_ACTION_MODIFIED:
                    result_array[i].type = FILE_EVENT_TYPE_MODIFIED;
                    break;

                default:
                    result_array[i].type = FILE_EVENT_TYPE_NULL;
                }

                if (notify->NextEntryOffset == 0)
                    break;
                notify = (FILE_NOTIFY_INFORMATION *)((u8 *)notify +
                                                     notify->NextEntryOffset);
            }
        }

        mem_zero_struct(&watcher->overlapped);
        ReadDirectoryChangesW(
            watcher->dir_handle, watcher->notification_buffer,
            sizeof(watcher->notification_buffer), watcher->scan_sub_directories,
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_DIR_NAME,
            NULL, &watcher->overlapped, NULL);
    }

    *out_count = count;
    return result_array;
}

function void file_watcher_destroy(FileWatcher *watcher) {
    debug_assert(watcher);
    CloseHandle(watcher->iocp);
    CloseHandle(watcher->dir_handle);
}

////////////////////////////////
// Module: Clock

function u64 clock_resolution_us(void) {
    LARGE_INTEGER resolution;
    if (QueryPerformanceFrequency(&resolution))
        return (resolution.QuadPart);
    return 1;
}

function u64 clock_ticks_now(void) {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (u64)(counter.QuadPart);
}

////////////////////////////////
// Module: Lib

function LibHandle lib_load(Str8 name) {
    LibHandle handle = {0};
    arena_scratch_scope(scratch) {
        Str16 name16 = str16_from_8(scratch, name);
        HMODULE mod = LoadLibraryW((LPWSTR)name16.data);
        handle.val[0] = (u64)mod;
    }
    return handle;
}

function void lib_unload(LibHandle handle) {
    FreeLibrary((HMODULE)handle.val[0]);
}

function void *lib_get_symbol(LibHandle lib, char *name) {
    return (void *)GetProcAddress((HMODULE)lib.val[0], (LPCSTR)name);
}