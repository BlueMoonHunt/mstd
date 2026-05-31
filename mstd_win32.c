#include "mstd.h"
#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "advapi32")

////////////////////////////////
// Module: Memory

function void* mem_reserve(u64 size, u32 large_pages) {
    void* result = (large_pages) ? VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE)
        : VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

function u8 mem_commit(void* ptr, u64 size, u32 large_pages) {
    u8 result = (large_pages) ? 1 : (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != NULL);
    return result;
}

function void mem_decommit(void* ptr, u64 size) {
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

function void mem_release(void* ptr, u64 size) {
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
}

function u64 mem_page_size(void) {
    SYSTEM_INFO sysinfo = { 0 };
    GetSystemInfo(&sysinfo);
    return sysinfo.dwPageSize;
}

function u64 mem_large_page_size(void) {
    return GetLargePageMinimum();
}

////////////////////////////////
// CLI

function void cli_attach_if_exists(void) {
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        HANDLE hOut = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        HANDLE hIn  = CreateFileA("CONIN$",  GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,  NULL, OPEN_EXISTING, 0, NULL);

        debug_assert(hOut != INVALID_HANDLE_VALUE);
        debug_assert(hIn != INVALID_HANDLE_VALUE);

        SetStdHandle(STD_OUTPUT_HANDLE, hOut);
        SetStdHandle(STD_ERROR_HANDLE,  hOut);
        SetStdHandle(STD_INPUT_HANDLE,  hIn);

        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
        freopen_s(&dummy, "CONIN$",  "r", stdin);

        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, mode);
        }
    }
}

////////////////////////////////
// Module: Thread

typedef enum ThreadEntityType {
    Thread_Entity_TYPE_NULL,
    Thread_Entity_TYPE_THREAD,
    Thread_Entity_TYPE_MUTEX,
    Thread_Entity_TYPE_RWMUTEX,
    Thread_Entity_TYPE_CONDITION_VARIABLE,
    Thread_Entity_TYPE_BARRIER,
}ThreadEntityType;

typedef struct ThreadEntity {
    struct ThreadEntity* next;
    enum_t(ThreadEntityType, u32) type;
    union {
        struct {
            ThreadEntryPointFunctionType* func;
            void* user_data;
            Handle handle;
            u32 id;
        }thread;
        #if OS_WINDOWS
            CRITICAL_SECTION mutex;
            SRWLOCK rw_mutex;
            CONDITION_VARIABLE cv;
            SYNCHRONIZATION_BARRIER sb;
        #endif
    };
}ThreadEntity;

typedef struct ThreadEntityState {
    Arena *arena;
    CRITICAL_SECTION mutex;
    ThreadEntity *head;
    ThreadEntity *tail;
}ThreadEntityState;

global ThreadEntityState thread_entity_state;

function DWORD WINAPI thread_entry_point(LPVOID lpParameter) {
    ThreadEntity* entity = (ThreadEntity*)lpParameter;

    if (!entity)
        return 1;

    ThreadEntryPointFunctionType* user_func = entity->thread.func;
    void* user_data = entity->thread.user_data;

    if (user_func)
        user_func(user_data);

    return (0);
}

function ThreadEntity* thread_entity_alloc(ThreadEntityType type) {
    ThreadEntity* result = 0;

    EnterCriticalSection(&thread_entity_state.mutex);
    {
        result = thread_entity_state.head;

        if(result)
            sll_stack_pop(thread_entity_state.head, thread_entity_state.tail);
        else
            result = arena_push_struct(thread_entity_state.arena, ThreadEntity);

        mem_zero_struct(result);
    }
    LeaveCriticalSection(&thread_entity_state.mutex);

    result->type = type;
    return result;
}

function void thread_entity_release(ThreadEntity* entity) {
    entity->type = Thread_Entity_TYPE_NULL;
    EnterCriticalSection(&thread_entity_state.mutex);
    {
        sll_stack_push(thread_entity_state.head, thread_entity_state.tail, entity);
    }
    LeaveCriticalSection(&thread_entity_state.mutex);
}

function Thread thread_attach(ThreadEntryPointFunctionType func, void* user_data) {
    ThreadEntity* entity = thread_entity_alloc(Thread_Entity_TYPE_THREAD);

    if (entity) {
        entity->thread.func = func;
        entity->thread.user_data = user_data;
        entity->thread.handle.val[0] = (u64)CreateThread(0, 0, thread_entry_point, entity, 0, (LPDWORD)&entity->thread.id);
    }

    Thread thread;
    thread.val[0] = (u64)entity;
    return thread;
}

function u32 thread_join(Thread thread) {
    ThreadEntity* entity = (ThreadEntity*)thread.val[0];
    DWORD wait_result = WaitForSingleObject((HANDLE)entity->thread.handle.val[0], INFINITE);
    if (entity &&  wait_result == WAIT_OBJECT_0) {
        CloseHandle((HANDLE)entity->thread.handle.val[0]);
        thread_entity_release(entity);
    }

    return (wait_result == WAIT_OBJECT_0);
}

function void thread_detach(Thread thread) {
    ThreadEntity* entity = (ThreadEntity*)thread.val[0];
    if(entity) {
        CloseHandle((HANDLE)entity->thread.handle.val[0]);
        thread_entity_release(entity);
    }
}

function u32 thread_id(void) {
    return (u32)GetCurrentThreadId();
}

function void thread_sleep(u32 ms) {
    Sleep((DWORD)ms);
}

function Mutex mutex_create(void) {
    ThreadEntity* entity = thread_entity_alloc(Thread_Entity_TYPE_MUTEX);
    if (entity)
        InitializeCriticalSection(&entity->mutex);
    Mutex mutex;
    mutex.val[0] = (u64)entity;
    return mutex;
}

function void mutex_take(Mutex mutex) {
    ThreadEntity* entity = (ThreadEntity*)mutex.val[0];
    EnterCriticalSection(&entity->mutex);
}

function void mutex_drop(Mutex mutex) {
    ThreadEntity* entity = (ThreadEntity*)mutex.val[0];
    LeaveCriticalSection(&entity->mutex);
}

function void mutex_destroy(Mutex mutex) {
    ThreadEntity* entity = (ThreadEntity*)mutex.val[0];
    DeleteCriticalSection(&entity->mutex);
    thread_entity_release(entity);
}

function RWMutex rw_mutex_create(void) {
    ThreadEntity* entity = thread_entity_alloc(Thread_Entity_TYPE_RWMUTEX);
    if (entity)
        InitializeSRWLock(&entity->rw_mutex);
    RWMutex rm;
    rm.val[0] = (u64)entity;
    return rm;
}

function void rw_mutex_take(RWMutex mutex, u32 write_mode) {
    ThreadEntity* entity = (ThreadEntity*)mutex.val[0];

    if (write_mode)
        AcquireSRWLockExclusive(&entity->rw_mutex);
    else
        AcquireSRWLockShared(&entity->rw_mutex);
}

function void rw_mutex_drop(RWMutex mutex, u32 write_mode) {
    ThreadEntity* entity = (ThreadEntity*)mutex.val[0];

    if (write_mode)
        ReleaseSRWLockExclusive(&entity->rw_mutex);
    else
        ReleaseSRWLockShared(&entity->rw_mutex);
}

function void rw_mutex_destroy(RWMutex mutex) {
    ThreadEntity* entity = (ThreadEntity*)mutex.val[0];
    thread_entity_release(entity);
}

function CondVar cond_var_create(void) {
    ThreadEntity* entity = thread_entity_alloc(Thread_Entity_TYPE_CONDITION_VARIABLE);
    if (entity)
        InitializeConditionVariable(&entity->cv);
    CondVar var;
    var.val[0] = (u64)entity;
    return var;
}

function u32 cond_var_wait(CondVar var, Mutex mutex) {
    u32 result = 0;
    ThreadEntity* cv_entity = (ThreadEntity*)var.val[0];
    ThreadEntity* m_entity = (ThreadEntity*)mutex.val[0];
    result = SleepConditionVariableCS(&cv_entity->cv, &m_entity->mutex, INFINITE);
    return result;
}

function u32 cond_var_wait_rw(CondVar var, RWMutex mutex, u32 write_mode) {
    u32 result = 0;
    ThreadEntity* cv_entity = (ThreadEntity*)var.val[0];
    ThreadEntity* m_entity = (ThreadEntity*)mutex.val[0];
    result = SleepConditionVariableSRW(&cv_entity->cv, &m_entity->rw_mutex, INFINITE, write_mode ? 0 : CONDITION_VARIABLE_LOCKMODE_SHARED);
    return result;
}

function void cond_var_signal(CondVar var) {
    ThreadEntity* entity = (ThreadEntity*)var.val[0];
    WakeConditionVariable(&entity->cv);
}

function void cond_var_broadcast(CondVar var) {
    ThreadEntity* entity = (ThreadEntity*)var.val[0];
    WakeAllConditionVariable(&entity->cv);
}

function void cond_var_destroy(CondVar var) {
    ThreadEntity* entity = (ThreadEntity*)var.val[0];
    thread_entity_release(entity);
}

function Semaphore semaphore_create(u32 initial_count, u32 max_count, Str8 name) {
    Semaphore semaphore;
    arena_scratch_scope(scratch) {
        Str16 name_w = str16_from_8(scratch, name);
        semaphore.val[0] = (u64)CreateSemaphoreW(0, initial_count, max_count, (LPCWSTR)name_w.data);
    }
    return semaphore;
}

function Semaphore semaphore_open(Str8 name) {
    Semaphore semaphore;
    arena_scratch_scope(scratch) {
        Str16 name_w = str16_from_8(scratch, name);
        semaphore.val[0] = (u64)OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, 0, (LPWSTR)name_w.data);
    }
    return semaphore;
}

function void semaphore_close(Semaphore semaphore) {
    CloseHandle((HANDLE)semaphore.val[0]);
}

function u32 semaphore_take(Semaphore semaphore) {
    DWORD wait_result = WaitForSingleObject((HANDLE)semaphore.val[0], INFINITE);
    return (wait_result == WAIT_OBJECT_0);
}

function void semaphore_drop(Semaphore semaphore) {
    ReleaseSemaphore((HANDLE)semaphore.val[0], 1, 0);
}

function void semaphore_destroy(Semaphore semaphore) {
    CloseHandle((HANDLE)semaphore.val[0]);
}

function Barrier barrier_create(u64 count) {
    ThreadEntity* entity = thread_entity_alloc(Thread_Entity_TYPE_BARRIER);
    if (entity) {
        InitializeSynchronizationBarrier(&entity->sb, count, -1);
    }
    Barrier barrier;
    barrier.val[0] = (u64)entity;
    return barrier;
}

function void barrier_wait(Barrier barrier) {
    ThreadEntity* entity = (ThreadEntity*)barrier.val[0];
    if (entity)
        EnterSynchronizationBarrier(&entity->sb, 0);
}

function void barrier_destroy(Barrier barrier) {
    ThreadEntity* entity = (ThreadEntity*)barrier.val[0];
    if (entity) {
        DeleteSynchronizationBarrier(&entity->sb);
        thread_entity_release(entity);
    }
}

function StripeArray stripe_array_alloc(Arena* arena) {
    StripeArray array = {0};

    SYSTEM_INFO info;
    GetSystemInfo(&info);

    array.count     = info.dwNumberOfProcessors;
    array.stripes   = arena_push_array(arena, Stripe, array.count);

    for (u32 i = 0; i < array.count; i++) {
        array.stripes[i].arena = arena_alloc(MB(64));
        array.stripes[i].rw_mutex = rw_mutex_create();
        array.stripes[i].cond_var = cond_var_create();
    }

    return array;
}

////////////////////////////////
// Module: File

function u32 file_delete(Str8 path) {
    int max_retries = 5;
    int sleep_ms = 1;

    for (int i = 0; i < max_retries; i++) {
        if (DeleteFileA((char*)path.data))
            return 1;

        DWORD err = GetLastError();
        if (err == ERROR_SHARING_VIOLATION || err == ERROR_LOCK_VIOLATION) {
            Sleep(sleep_ms);
            sleep_ms *= 2;
        }
        else
            break;
    }
    return 0;
}

function u32 file_copy(Str8 src, Str8 dest) {
    int max_retries = 5;
    int sleep_ms = 1;

    for (int i = 0; i < max_retries; i++) {
        if (CopyFileExA((char*)src.data, (char*)dest.data, NULL, NULL, FALSE, COPY_FILE_NO_BUFFERING))
            return 1;

        DWORD err = GetLastError();
        if (err == ERROR_SHARING_VIOLATION || err == ERROR_LOCK_VIOLATION) {
            Sleep(sleep_ms);
            sleep_ms *= 2;
        }
        else
            break;
    }

    return 0;
}

function u32 file_move(Str8 src, Str8 dest) {
    int max_retries = 5;
    int sleep_ms = 1;

    for (int i = 0; i < max_retries; i++) {
        if (MoveFileExA((char*)src.data, (char*)dest.data, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH)) {
            return 1;
        }

        DWORD err = GetLastError();
        if (err == ERROR_SHARING_VIOLATION || err == ERROR_LOCK_VIOLATION) {
            Sleep(sleep_ms);
            sleep_ms *= 2;
        }
        else
            break;
    }
    return 0;
}

function u32 file_exists(Str8 path) {
    DWORD attributes = GetFileAttributesA((char*)path.data);
    u32 exists = (attributes != INVALID_FILE_ATTRIBUTES) && !!(~attributes & FILE_ATTRIBUTE_DIRECTORY);
    return exists;
}

function u32 file_directory_exists(Str8 path) {
    DWORD attributes = GetFileAttributesA((char*)path.data);
    u32 exists = (attributes != INVALID_FILE_ATTRIBUTES) && (attributes & FILE_ATTRIBUTE_DIRECTORY);
    return exists;
}

function FileHandle file_open(Str8 name, FileAccessFlag flags) {
    FileHandle result = { 0 };
    Arena* arena = arena_scratch_alloc();
    Str16 path_w32 = str16_from_8(arena, name);
    DWORD access_flags = 0;
    DWORD share_mode = 0;
    DWORD creation_disposition = OPEN_EXISTING;

    if (flags & FILE_ACCESS_FLAG_READ)          { access_flags |= GENERIC_READ; }
    if (flags & FILE_ACCESS_FLAG_WRITE)         { access_flags |= GENERIC_WRITE; creation_disposition = CREATE_ALWAYS; }
    if (flags & FILE_ACCESS_FLAG_EXECUTE)       { access_flags |= GENERIC_EXECUTE; }
    if (flags & FILE_ACCESS_FLAG_SHARE_READ)    { share_mode |= FILE_SHARE_READ; }
    if (flags & FILE_ACCESS_FLAG_SHARE_WRITE)   { share_mode |= FILE_SHARE_WRITE | FILE_SHARE_DELETE; }
    if (flags & FILE_ACCESS_FLAG_APPEND)        { access_flags &= ~GENERIC_WRITE; access_flags |= FILE_APPEND_DATA; creation_disposition = OPEN_ALWAYS; }

    HANDLE file = CreateFileW((WCHAR*)path_w32.data, access_flags, share_mode, 0, creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);
    if (file != INVALID_HANDLE_VALUE)
        result.val[0] = (u64)file;
    arena_scratch_release(arena);
    debug_assert(result.val[0]);

    return result;
}

function u64 file_size(FileHandle handle) {
    u64 size = 0;
    GetFileSizeEx((HANDLE)handle.val[0], (LARGE_INTEGER*)&size);
    return size;
}

function void file_close(FileHandle handle) {
    CloseHandle((HANDLE)handle.val[0]);
}

function u8* file_read_ex(Arena* arena, FileHandle handle, u64 offset, u64 size) {
    u64 total_read_size = 0;

    if (handle.val[0]) {
        HANDLE file = (HANDLE)handle.val[0];
        u64 _size = 0;
        GetFileSizeEx(file, (LARGE_INTEGER*)&_size);

        u64 total_to_read = clamp_top(size, _size);
        u8* out = arena_push_array(arena, u8, total_to_read);

        for (u64 _offset = offset; total_read_size < total_to_read; ) {
            u64 remaining = total_to_read - total_read_size;
            DWORD to_read = (DWORD)clamp_top(remaining, u32_max);
            DWORD actual_read = 0;

            OVERLAPPED overlapped = { 0 };
            overlapped.Offset = (DWORD)(_offset & u32_max);
            overlapped.OffsetHigh = (DWORD)(_offset >> 32);

            if(ReadFile(file, (u8*)out + total_read_size, to_read, &actual_read, &overlapped)) {
                _offset += actual_read;
                total_read_size += actual_read;
                if (actual_read < to_read)
                    break;
            }
            else
                break;
        }
        return out;
    }


    return 0;
}

function void file_write_ex(FileHandle handle, void* data, u64 offset, u64 size) {
    u64 total_written = 0;

    if (handle.val[0]) {
        for (u64 dest_offset = offset;;) {

            u64 remaining = size - total_written;
            if (remaining == 0) break;

            DWORD to_write = (DWORD)clamp_top(remaining, MB(1));
            DWORD actual_write = 0;

            OVERLAPPED overlapped = { 0 };
            overlapped.Offset = (DWORD)(dest_offset & 0xFFFFFFFF);
            overlapped.OffsetHigh = (DWORD)(dest_offset >> 32);

            if (!WriteFile((HANDLE)handle.val[0], (u8*)data + total_written, to_write, &actual_write, &overlapped))
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
    Arena* arena = arena_scratch_alloc();
    Str16 u16_path = str16_from_8(arena, path);

    HANDLE dir = CreateFileW((LPCWSTR)u16_path.data, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);


    struct Win32FileWatcher watcher = { 0 };

    if (dir == INVALID_HANDLE_VALUE)
        return(watcher);

    watcher.scan_sub_directories = watch_sub_directory;
    watcher.dir_handle = dir;
    watcher.iocp = CreateIoCompletionPort(watcher.dir_handle, NULL, 0, 1);

    ReadDirectoryChangesW(
        watcher.dir_handle,
        watcher.notification_buffer,
        sizeof(watcher.notification_buffer),
        watcher.scan_sub_directories,
        FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
        NULL,
        &watcher.overlapped,
        NULL
    );

    arena_scratch_release(arena);
    return(watcher);
}

function FileEvent* file_watcher_poll_events(FileWatcher* watcher, Arena* arena, u32 timeout_ms, u32* out_count) {
    DWORD bytes = 0;
    ULONG_PTR key = 0;
    LPOVERLAPPED p_overlapped = NULL;

    FileEvent* result_array = NULL;
    u32 count = 0;

    if (GetQueuedCompletionStatus(watcher->iocp, &bytes, &key, &p_overlapped, (DWORD)timeout_ms)) {
        if (p_overlapped == &watcher->overlapped && bytes > 0) {

            FILE_NOTIFY_INFORMATION* notify = (FILE_NOTIFY_INFORMATION*)watcher->notification_buffer;
            for (;;) {
                count++;
                if (notify->NextEntryOffset == 0) break;
                notify = (FILE_NOTIFY_INFORMATION*)((u8*)notify + notify->NextEntryOffset);
            }

            result_array = arena_push_array(arena, FileEvent, count);

            notify = (FILE_NOTIFY_INFORMATION*)watcher->notification_buffer;

            for (u32 i = 0; i < count; i++) {
                u32 name_len_chars = notify->FileNameLength / sizeof(u16);
                Str16 u16_file = { (u16*)notify->FileName, name_len_chars };

                result_array[i].file_name = str8_from_16(arena, u16_file);
                switch (notify->Action) {
                case FILE_ACTION_ADDED:
                case FILE_ACTION_RENAMED_NEW_NAME:      result_array[i].type = FILE_EVENT_TYPE_ADDED; break;

                case FILE_ACTION_REMOVED:
                case FILE_ACTION_RENAMED_OLD_NAME:      result_array[i].type = FILE_EVENT_TYPE_DELETED; break;

                case FILE_ACTION_MODIFIED:              result_array[i].type = FILE_EVENT_TYPE_MODIFIED; break;

                default:                                result_array[i].type = FILE_EVENT_TYPE_NULL;
                }

                if (notify->NextEntryOffset == 0) break;
                notify = (FILE_NOTIFY_INFORMATION*)((u8*)notify + notify->NextEntryOffset);
            }
        }

        mem_zero_struct(&watcher->overlapped);
        ReadDirectoryChangesW(watcher->dir_handle, watcher->notification_buffer, sizeof(watcher->notification_buffer),
            watcher->scan_sub_directories,
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME,
            NULL, &watcher->overlapped, NULL);
    }

    *out_count = count;
    return result_array;
}

function void file_watcher_destroy(FileWatcher* watcher) {
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

function void*lib_get_symbol(LibHandle lib, char* name) {
    return (void*)GetProcAddress((HMODULE)lib.val[0], (LPCSTR)name);
}