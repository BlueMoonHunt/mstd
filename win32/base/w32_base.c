internal force_inline void* mem_reserve(U64 size, U8 large) {
    void* result = (large)  ? VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE)
                            : VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

internal force_inline void mem_commit(void* mem, U64 size, U8 large) {
    if (!large)
        VirtualAlloc(mem, size, MEM_COMMIT, PAGE_READWRITE);
}

internal force_inline void mem_decommit(void* mem, U64 size) {
    VirtualFree(mem, size, MEM_DECOMMIT);
}

internal force_inline void mem_release(void* mem, U64 size) {
    (void)size;
    VirtualFree(mem, size, MEM_RELEASE);
}

internal force_inline U64 mem_page_size(void) {
    SYSTEM_INFO info;

    GetSystemInfo(&info);
    return (U64)info.dwPageSize;
}

/* State */

internal void w32_system_info_fetch(void) {
    SYSTEM_INFO sys_info;
    LARGE_INTEGER resolution;

    GetSystemInfo(&sys_info);
    w32_state.system.page_size                  = sys_info.dwPageSize;
    w32_state.system.large_page_size            = GetLargePageMinimum();
    w32_state.system.logical_processor_count    = sys_info.dwNumberOfProcessors;
    w32_state.system.allocation_granularity     = sys_info.dwAllocationGranularity;

    w32_state.system.microsecond_resolution = 1;
    if (QueryPerformanceFrequency(&resolution))
        w32_state.system.microsecond_resolution = resolution.QuadPart;
}

internal void w32_process_info_fetch(void) {
    Arena* scratch;
    U32 buffer_size;
    U32 char_count;
    Str16 path_cwd;
    Str16 path_bin;

    scratch = arena_scratch_alloc();
    buffer_size = KB(32);
    char_count = GetCurrentDirectoryW(0, 0);

    path_bin = str16_from_size(scratch, buffer_size - 1);
    path_bin.size = GetModuleFileNameW(0, path_bin.data, buffer_size);
    path_bin.data[path_bin.size] = 0;

    if (char_count) {
        path_cwd = str16_from_size(scratch, char_count - 1);
        path_cwd.size = GetCurrentDirectoryW(path_cwd.size, path_cwd.data);
        path_cwd.data[path_cwd.size] = 0;
    }
    else {
        path_cwd.data = 0;
        path_cwd.size = 0;
    }

    w32_state.process.bin_path                   = str8_from_str16(w32_state.arena, path_bin);
    w32_state.process.id                         = GetCurrentProcessId();
    w32_state.process.current_working_directory  = str8_from_str16(w32_state.arena, path_cwd);

    arena_scratch_release(scratch);
}

LRESULT CALLBACK w32_message_callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

internal void os_state_init() {
    SYSTEM_INFO sys_info;
    LARGE_INTEGER resolution;
    Arena* scratch;
    U32 buffer_size;
    Str16 path_cwd;
    Str16 path_bin;

    GetSystemInfo(&sys_info);
    w32_state.system.page_size                  = sys_info.dwPageSize;
    w32_state.system.large_page_size            = GetLargePageMinimum();
    w32_state.system.logical_processor_count    = sys_info.dwNumberOfProcessors;
    w32_state.system.allocation_granularity     = sys_info.dwAllocationGranularity;

    w32_state.system.microsecond_resolution = 1;
    if (QueryPerformanceFrequency(&resolution))
        w32_state.system.microsecond_resolution = resolution.QuadPart;

    w32_state.arena = arena_alloc();

    scratch     = arena_scratch_alloc();
    buffer_size = KB(32);

    path_bin                        = str16_from_size(scratch, buffer_size - 1);
    path_bin.size                   = GetModuleFileNameW(0, path_bin.data, buffer_size);
    path_bin.data[path_bin.size]    = 0;

    path_cwd                        = str16_from_size(scratch, buffer_size - 1);
    path_cwd.size                   = GetCurrentDirectoryW(path_cwd.size, path_cwd.data);
    path_cwd.data[path_cwd.size]    = 0;

    w32_state.process.bin_path                   = str8_from_str16(w32_state.arena, path_bin);
    w32_state.process.id                         = GetCurrentProcessId();
    w32_state.process.current_working_directory  = str8_from_str16(w32_state.arena, path_cwd);

    InitializeCriticalSection(&w32_state.entity_mutex);

    arena_scratch_release(scratch);
}

internal SystemInfo* system_info(void) {
    return &w32_state.system;
}

internal ProcessInfo* process_info(void) {
    return &w32_state.process;
}


/* Entity */

internal W32_Entity* w32_entity_alloc(W32_EntityType type) {
    W32_Entity* entity;

    EnterCriticalSection(&w32_state.entity_mutex);
    entity = w32_state.entity_stack_head;

    if (entity)
        sll_stack_pop_ht(w32_state.entity_stack_head, w32_state.entity_stack_tail);
    else
        entity = arena_push_type(w32_state.arena, W32_Entity);

    mem_zero_type(entity);
    LeaveCriticalSection(&w32_state.entity_mutex);
    entity->type = type;
    return entity;
}

internal void w32_entity_release(W32_Entity* entity) {
    entity->type = W32_ENTITY_TYPE_UNKNOWN;
    EnterCriticalSection(&w32_state.entity_mutex);
    sll_stack_push_ht(w32_state.entity_stack_head, w32_state.entity_stack_tail, entity);
    LeaveCriticalSection(&w32_state.entity_mutex);
}

/* Thread */

internal DWORD w32_thread_entrypoint(void* ptr) {
    W32_Entity* entity;

    entity = (W32_Entity*)ptr;
    thread_entrypoint(entity->thread.func, entity->thread.data);

    return 0;
}

internal Handle thread_create(ThreadEntryFunction* func, void* data) {
    W32_Entity* entity;
    Handle thread;

    entity = w32_entity_alloc(W32_ENTITY_TYPE_THREAD);

    entity->thread.func     = func;
    entity->thread.data     = data;
    entity->thread.handle   = CreateThread(0, 0, w32_thread_entrypoint, entity, 0, &entity->thread.id);

    thread.val[0] = (U64)entity;

    return thread;
}

internal U32 thread_join(Handle thread, U64 end_us) {
    W32_Entity* entity;
    U32 sleep_ms;
    U32 wait;

    entity  = (W32_Entity*)thread.val[0];
    wait    = WAIT_OBJECT_0;

    if (entity) {
        sleep_ms = w32_sleep_ms_from_end_us(end_us);
        wait = WaitForSingleObject(entity->thread.handle, sleep_ms);

        CloseHandle(entity->thread.handle);
        w32_entity_release(entity);
    }

    return (wait == WAIT_OBJECT_0);
}

internal void thread_destroy(Handle thread) {
    W32_Entity* entity;

    entity = (W32_Entity*)thread.val[0];

    if (entity) {
        CloseHandle(entity->thread.handle);
        w32_entity_release(entity);
    }
}

internal U32 thread_id(void) {
    return (U32)GetCurrentThreadId();
}

internal void thread_sleep(U64 us) {
    LARGE_INTEGER due;

    if (!us) {
        SwitchToThread();
        return;
    }

    if (!w32_thread_timer)
        w32_thread_timer = CreateWaitableTimerExW(0, 0, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);

    if (w32_thread_timer) {
        due.QuadPart = -(LONGLONG)(us * 10);
        SetWaitableTimer(w32_thread_timer, &due, 0, 0, 0, FALSE);
        WaitForSingleObject(w32_thread_timer, INFINITE);
    }
    else {
        Sleep(us / 1000);
    }
}

/* Mutex */

internal Handle mutex_create(void) {
    W32_Entity* entity;
    Handle mutex;

    entity = w32_entity_alloc(W32_ENTITY_TYPE_MUTEX);
    InitializeCriticalSection(&entity->mutex);
    mutex.val[0] = (U64)entity;

    return mutex;
}

internal void mutex_destroy(Handle mutex) {
    W32_Entity* entity;

    entity = (W32_Entity*)mutex.val[0];
    DeleteCriticalSection(&entity->mutex);
    w32_entity_release(entity);
}

internal void mutex_take(Handle mutex) {
    W32_Entity* entity;

    entity = (W32_Entity*)mutex.val[0];
    EnterCriticalSection(&entity->mutex);
}

internal void mutex_drop(Handle mutex) {
    W32_Entity* entity;

    entity = (W32_Entity*)mutex.val[0];
    LeaveCriticalSection(&entity->mutex);
}

/* RWMutex */

internal Handle rw_mutex_create(void) {
    W32_Entity* entity;
    Handle rw_mutex;

    entity = w32_entity_alloc(W32_ENTITY_TYPE_RWMUTEX);
    InitializeSRWLock(&entity->rw_mutex);
    rw_mutex.val[0] = (U64)entity;

    return rw_mutex;
}

internal void rw_mutex_destroy(Handle rw_mutex) {
    W32_Entity* entity;

    entity = (W32_Entity*)rw_mutex.val[0];
    w32_entity_release(entity);
}

internal void rw_mutex_take(Handle rw_mutex, U32 write_mode) {
    W32_Entity* entity;

    entity = (W32_Entity*)rw_mutex.val[0];

    if (write_mode)
        AcquireSRWLockExclusive(&entity->rw_mutex);
    else
        AcquireSRWLockShared(&entity->rw_mutex);
}

internal void rw_mutex_drop(Handle rw_mutex, U32 write_mode) {
    W32_Entity* entity;

    entity = (W32_Entity*)rw_mutex.val[0];

    if (write_mode)
        ReleaseSRWLockExclusive(&entity->rw_mutex);
    else
        ReleaseSRWLockShared(&entity->rw_mutex);
}

/* CondVar */

internal Handle cond_var_create(void) {
    W32_Entity* entity;
    Handle var;

    entity = w32_entity_alloc(W32_ENTITY_TYPE_COND_VAR);
    InitializeConditionVariable(&entity->cond_var);
    var.val[0] = (U64)entity;

    return var;
}

internal void cond_var_destroy(Handle cond_var) {
    W32_Entity* entity;

    entity = (W32_Entity*)cond_var.val[0];
    w32_entity_release(entity);
}

internal U32 cond_var_wait(Handle cond_var, Handle mutex, U64 end_us) {
    W32_Entity* entity_cv;
    W32_Entity* entity_mx;
    U32 sleep_ms;
    U32 result;

    result      = 0;
    entity_cv   = (W32_Entity*)cond_var.val[0];
    entity_mx   = (W32_Entity*)mutex.val[0];
    sleep_ms    = w32_sleep_ms_from_end_us(end_us);

    if (sleep_ms)
        result = SleepConditionVariableCS(&entity_cv->cond_var, &entity_mx->mutex, sleep_ms);

    return result;
}

internal U32 cond_var_wait_rw(Handle cond_var, Handle rw_mutex, U32 write_mode, U64 end_us) {
    W32_Entity* entity_cv;
    W32_Entity* entity_mx;
    U32 sleep_ms;
    U32 result;

    result      = 0;
    entity_cv   = (W32_Entity*)cond_var.val[0];
    entity_mx   = (W32_Entity*)rw_mutex.val[0];
    sleep_ms    = w32_sleep_ms_from_end_us(end_us);

    if (sleep_ms)
        result = SleepConditionVariableSRW(&entity_cv->cond_var, &entity_mx->rw_mutex, sleep_ms, write_mode ? 0 : CONDITION_VARIABLE_LOCKMODE_SHARED);

    return result;
}

internal void cond_var_wake(Handle cond_var) {
    W32_Entity* entity;

    entity = (W32_Entity*)cond_var.val[0];
    WakeConditionVariable(&entity->cond_var);
}

internal void cond_var_wake_all(Handle cond_var) {
    W32_Entity* entity;

    entity = (W32_Entity*)cond_var.val[0];
    WakeAllConditionVariable(&entity->cond_var);
}

/* Semaphore */

internal Handle semaphore_create(U32 initial_count, U32 max_count, Str8 name) {
    Arena* arena;
    Str16 name_16;
    Handle semaphore;

    arena_scratch_scope(arena) {
        name_16 = str16_from_str8(arena, name);
        semaphore.val[0] = (U64)CreateSemaphoreW(0, initial_count, max_count, (WCHAR*)name_16.data);
    }

    return semaphore;
}

internal void semaphore_destroy(Handle semaphore) {
    CloseHandle((HANDLE)semaphore.val[0]);
}

internal Handle semaphore_open(Str8 name) {
    Arena* arena;
    Str16 name_16;
    Handle semaphore;

    arena_scratch_scope(arena) {
        name_16 = str16_from_str8(arena, name);
        semaphore.val[0] = (U64)OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, 0, (WCHAR*)name_16.data);
    }

    return semaphore;
}

internal void semaphore_close(Handle semaphore) {
    CloseHandle((HANDLE)semaphore.val[0]);
}

internal U32 semaphore_take(Handle semaphore, U64 end_us) {
    U32 sleep_ms;
    U32 result;

    sleep_ms    = w32_sleep_ms_from_end_us(end_us);
    result      = WaitForSingleObject((HANDLE)semaphore.val[0], sleep_ms);

    return (result == WAIT_OBJECT_0);
}

internal void semaphore_drop(Handle semaphore, U64 count) {
    ReleaseSemaphore((HANDLE)semaphore.val[0], count, 0);
}

/* Barrier */

internal Handle barrier_create(U32 count) {
    Handle barrier;
    W32_Entity* entity;

    entity = w32_entity_alloc(W32_ENTITY_TYPE_BARRIER);
    InitializeSynchronizationBarrier(&entity->synchronization_barrier, count, -1);
    barrier.val[0] = (U64)entity;

    return barrier;
}

internal void barrier_destroy(Handle barrier) {
    W32_Entity* entity;

    entity = (W32_Entity*)barrier.val[0];
    DeleteSynchronizationBarrier(&entity->synchronization_barrier);
}

internal void barrier_wait(Handle barrier) {
    W32_Entity* entity;

    entity = (W32_Entity*)barrier.val[0];
    EnterSynchronizationBarrier(&entity->synchronization_barrier, 0);
}

/* Wait on address */

internal void atomic_wait_U32(volatile U32* addr, U32 expected_value, U64 end_us) {
    U32 sleep_ms;
    sleep_ms = w32_sleep_ms_from_end_us(end_us);

    if (sleep_ms)
        WaitOnAddress((volatile PVOID)addr, (PVOID)&expected_value, sizeof(U32), sleep_ms);
}

internal void atomic_wake(volatile U32* addr) {
    WakeByAddressSingle((volatile PVOID)addr);
}

internal void atomic_wake_all(volatile U32* addr) {
    WakeByAddressAll((volatile PVOID)addr);
}

/* File */

internal U32 file_delete(Str8 path) {
    U32 i;
    U32 sleep_us;
    U32 err;
    Arena* arena;
    Str16 path_16;

    sleep_us    = 500;
    arena       = arena_scratch_alloc();
    path_16     = str16_from_str8(arena, path);

    for_each_index(i, FILE_MAX_RETRIES) {
        if (DeleteFileW(path_16.data))
            return 1;

        err = GetLastError();
        if (err != ERROR_SHARING_VIOLATION && err != ERROR_LOCK_VIOLATION)
            break;

        thread_sleep(sleep_us);
        sleep_us *= 2;
    }

    arena_scratch_release(arena);

    return 0;
}

internal U32 file_copy(Str8 src, Str8 dest) {
    U32 i;
    U32 sleep_us;
    U32 err;
    Arena* arena;
    Str16 src_16;
    Str16 dest_16;

    sleep_us    = 500;
    arena       = arena_scratch_alloc();
    src_16      = str16_from_str8(arena, src);
    dest_16     = str16_from_str8(arena, dest);


    for_each_index(i, FILE_MAX_RETRIES) {
        if (CopyFileExW(src_16.data, dest_16.data, 0, 0, FALSE, 0))
            return 1;

        err = GetLastError();
        if (err != ERROR_SHARING_VIOLATION && err != ERROR_ACCESS_DENIED && err != ERROR_LOCK_VIOLATION)
            break;

        thread_sleep(sleep_us);
        sleep_us *= 2;
    }

    arena_scratch_release(arena);

    return 0;
}

internal U32 file_move(Str8 src, Str8 dest) {
    U32 i;
    U32 sleep_us;
    U32 err;
    Arena* arena;
    Str16 src_16;
    Str16 dest_16;

    sleep_us    = 500;
    arena       = arena_scratch_alloc();
    src_16      = str16_from_str8(arena, src);
    dest_16     = str16_from_str8(arena, dest);

    for_each_index(i, FILE_MAX_RETRIES) {
        if (MoveFileExW(src_16.data, dest_16.data, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH))
            return 1;

        err = GetLastError();
        if (err != ERROR_SHARING_VIOLATION && err != ERROR_LOCK_VIOLATION)
            break;

        thread_sleep(sleep_us);
        sleep_us *= 2;
    }

    arena_scratch_release(arena);

    return 0;
}

internal U32 file_exists(Str8 path) {
    Arena* arena;
    Str16 path_16;
    U32 attributes;
    U32 exists;

    arena   = arena_scratch_alloc();
    path_16 = str16_from_str8(arena, path);

    attributes = GetFileAttributesW(path_16.data);
    exists = (attributes != INVALID_FILE_ATTRIBUTES) && !!(~attributes & FILE_ATTRIBUTE_DIRECTORY);

    arena_scratch_release(arena);

    return exists;
}

internal U32 file_directory_exists(Str8 path) {
    Arena* arena;
    Str16 path_16;
    U32 attributes;
    U32 exists;

    arena   = arena_scratch_alloc();
    path_16 = str16_from_str8(arena, path);


    attributes = GetFileAttributesW(path_16.data);
    exists = (attributes != INVALID_FILE_ATTRIBUTES) && (attributes & FILE_ATTRIBUTE_DIRECTORY);

    arena_scratch_release(arena);

    return exists;
}

internal U64 file_size_get(Handle file) {
    LARGE_INTEGER size;
    HANDLE handle;

    handle = (HANDLE)file.val[0];

    if (!GetFileSizeEx(handle, &size))
        return 0;

    return (U64)size.QuadPart;
}

internal Handle file_open(Str8 name, FileAccessFlag flags) {
    Handle result;
    Arena* arena;
    Str16 path_16;
    U32 access_flags;
    U32 share_mode;
    U32 creation_flag;
    HANDLE file;

    access_flags    = 0;
    share_mode      = 0;
    result.val[0]   = 0;
    arena           = arena_scratch_alloc();
    path_16         = str16_from_str8(arena, name);
    creation_flag   = OPEN_EXISTING;


    if (flags & FILE_ACCESS_FLAG_READ)          { access_flags |= GENERIC_READ; }
    if (flags & FILE_ACCESS_FLAG_SHARE_READ)    { share_mode |= FILE_SHARE_READ; }
    if (flags & FILE_ACCESS_FLAG_EXECUTE)       { access_flags |= GENERIC_EXECUTE; }
    if (flags & FILE_ACCESS_FLAG_SHARE_WRITE)   { share_mode |= FILE_SHARE_WRITE | FILE_SHARE_DELETE; }
    if (flags & FILE_ACCESS_FLAG_WRITE)         { access_flags |= GENERIC_WRITE; creation_flag = CREATE_ALWAYS; }
    if (flags & FILE_ACCESS_FLAG_APPEND)        { access_flags &= ~GENERIC_WRITE; access_flags |= FILE_APPEND_DATA; creation_flag = OPEN_ALWAYS; }

    file = CreateFileW((WCHAR*)path_16.data, access_flags, share_mode, 0, creation_flag, FILE_ATTRIBUTE_NORMAL, 0);

    if (file != INVALID_HANDLE_VALUE)
        result.val[0] = (U64)file;

    arena_scratch_release(arena);

    return result;
}

internal void file_close(Handle handle) {
    CloseHandle((HANDLE)handle.val[0]);
}

internal Str8 file_read(Arena *arena, Handle handle, U64 size, U64 offset) {
    U32 to_read;
    DWORD actual_read;
    U64 remaining;
    U64 total_read_size;
    U64 _size;
    U64 total_to_read;
    Str8 str;
    OVERLAPPED overlapped;
    U8 *out;

    total_read_size = 0;
    str.data = 0;
    str.size = 0;
    _size = 0;

    if (GetFileSizeEx((HANDLE)handle.val[0], (LARGE_INTEGER*)&_size)) {
        total_to_read = clamp_top(size, _size);
        out = arena_push_array(arena, U8, total_to_read + 1);

        for (; total_read_size < total_to_read;) {
            remaining = total_to_read - total_read_size;
            to_read = (U32)clamp_top(remaining, U32_MAX);
            actual_read = 0;

            mem_zero_type(&overlapped);
            overlapped.Offset = (U32)(offset & U32_MAX);
            overlapped.OffsetHigh = (U32)(offset >> 32);

            if (ReadFile((HANDLE)handle.val[0], out + total_read_size, to_read, &actual_read, &overlapped)) {
                offset += actual_read;
                total_read_size += actual_read;
                if (actual_read < to_read) {
                    break;
                }
            } else {
                break;
            }
        }
        out[total_read_size] = 0;

        str.size = total_read_size;
        str.data = out;
    }

    return str;
}

internal void file_write(Handle handle, void *data, U64 size, U64 offset) {
    U32 to_write;
    DWORD actual_write;
    U64 total_written;
    U64 remaining;
    OVERLAPPED overlapped;

    for (total_written = 0;;) {
        remaining = size - total_written;
        if (!remaining)
            break;

        to_write = (U32)clamp_top(remaining, U32_MAX);
        actual_write = 0;

        mem_zero_type(&overlapped);
        overlapped.Offset = (U32)(offset & U32_MAX);
        overlapped.OffsetHigh = (U32)(offset >> 32);

        if (!WriteFile((HANDLE)handle.val[0], (U8*)data + total_written, to_write, &actual_write, &overlapped)) {
            break;
        }
        total_written += actual_write;
        offset += actual_write;
    }
}

/* File Watcher */

internal Handle file_watcher_create(Arena* arena, Str8 path, U32 watch_sub_directory) {
    Handle watcher = {0};
    Arena* scratch;
    Str16 path_16;
    HANDLE dir;
    W32_FileWatcher* w32_watcher;
    DWORD access;
    DWORD share_mode;
    DWORD attributes;

    scratch    = arena_scratch_alloc();
    path_16    = str16_from_str8(scratch, path);
    access     = FILE_LIST_DIRECTORY;
    share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    attributes = FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED;

    dir = CreateFileW((LPCWSTR)path_16.data, access, share_mode, 0, OPEN_EXISTING, attributes, 0);

    if (dir != INVALID_HANDLE_VALUE) {
        w32_watcher = arena_push_type(arena, W32_FileWatcher);

        w32_watcher->dir_handle           = dir;
        w32_watcher->scan_sub_directories = watch_sub_directory;
        w32_watcher->iocp                 = CreateIoCompletionPort(dir, 0, 0, 1);

        mem_zero_type(&w32_watcher->overlapped);
        ReadDirectoryChangesW(dir, w32_watcher->notification_buffer, sizeof(w32_watcher->notification_buffer),
            watch_sub_directory, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME,
            0, &w32_watcher->overlapped, 0);

        watcher.val[0] = (U64)w32_watcher;
    }

    arena_scratch_release(scratch);
    return watcher;
}

internal void file_watcher_poll_events(Arena* arena, Handle watcher, FileEvents* events, U32 timeout_ms) {
    FILE_NOTIFY_INFORMATION* notify;
    W32_FileWatcher* w32_watcher;
    OVERLAPPED* overlapped;
    DWORD byte;
    U32 count;
    U32 index;
    U64 key;
    Str16 event_file;

    count       = 0;
    index       = 0;
    w32_watcher = (W32_FileWatcher*)watcher.val[0];

    if (!w32_watcher) return;

    mem_zero(events, sizeof(*events));

    if (GetQueuedCompletionStatus(w32_watcher->iocp, &byte, &key, &overlapped, (DWORD)timeout_ms) &&
        overlapped == &w32_watcher->overlapped) {

        if (byte > 0) {
            notify = (FILE_NOTIFY_INFORMATION*)w32_watcher->notification_buffer;

            for (count = 0; notify != 0; ) {
                count++;
                if (notify->NextEntryOffset == 0) break;
                notify = (FILE_NOTIFY_INFORMATION*)((U8*)notify + notify->NextEntryOffset);
            }

            events->count      = count;
            events->file_array = arena_push_array(arena, Str8, count);
            events->type_array = arena_push_array(arena, FileEventType, count);

            notify = (FILE_NOTIFY_INFORMATION*)w32_watcher->notification_buffer;

            for_each_index(index, count) {
                event_file.data = notify->FileName;
                event_file.size = notify->FileNameLength / sizeof(U16);

                events->file_array[index] = str8_from_str16(arena, event_file);

                switch (notify->Action) {
                    case FILE_ACTION_ADDED:
                    case FILE_ACTION_RENAMED_NEW_NAME: {
                        events->type_array[index] = FILE_EVENT_TYPE_ADDED;
                    } break;

                    case FILE_ACTION_REMOVED:
                    case FILE_ACTION_RENAMED_OLD_NAME: {
                        events->type_array[index] = FILE_EVENT_TYPE_DELETED;
                    } break;

                    case FILE_ACTION_MODIFIED: {
                        events->type_array[index] = FILE_EVENT_TYPE_MODIFIED;
                    } break;

                    default: {
                        events->type_array[index] = FILE_EVENT_TYPE_NULL;
                    } break;
                }

                if (notify->NextEntryOffset == 0) break;
                notify = (FILE_NOTIFY_INFORMATION*)((U8*)notify + notify->NextEntryOffset);
            }
        }

        mem_zero_type(&w32_watcher->overlapped);
        ReadDirectoryChangesW(w32_watcher->dir_handle, w32_watcher->notification_buffer, sizeof(w32_watcher->notification_buffer),
            w32_watcher->scan_sub_directories, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME,
            0, &w32_watcher->overlapped, 0);
    }
}

internal void file_watcher_destroy(Handle watcher) {
    W32_FileWatcher* w32_watcher = (W32_FileWatcher*)watcher.val[0];
    CancelIoEx(w32_watcher->dir_handle, &w32_watcher->overlapped);
    CloseHandle(w32_watcher->dir_handle);
    CloseHandle(w32_watcher->iocp);
}

/* Time */

internal U64 ticks_now(void) {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (U64)(counter.QuadPart);
}

internal U64 time_now_us(void) {
    U64 result;
    LARGE_INTEGER counter;

    result = 0;
    if (QueryPerformanceCounter(&counter))
        result = (counter.QuadPart * 1000000) / system_info()->microsecond_resolution;

    return result;
}

internal U32 w32_sleep_ms_from_end_us(U64 end_us) {
    U32 begin_us;
    U32 sleep_ms;

    sleep_ms = 0;

    if (end_us == U64_MAX) {
        sleep_ms = INFINITE;
    }
    else {
        begin_us = time_now_us();
        if (begin_us < end_us) {
            sleep_ms = (U32)((end_us - begin_us + 999)/1000);
        }
    }

    return sleep_ms;
}

/* Lib */

internal Handle lib_load_(Str8 lib_name) {
    Handle handle;
    Str16 name16;
    Arena* arena;

    arena = arena_scratch_alloc();
    name16 = str16_from_str8(arena, lib_name);
    handle.val[0] = (U64)LoadLibraryW((LPWSTR)name16.data);
    arena_scratch_release(arena);

    return handle;
}

internal void* lib_symbol_get_(Handle lib, Str8 symbol_name) {
    return (void *)GetProcAddress((HMODULE)lib.val[0], (LPCSTR)symbol_name.data);
}

internal void lib_unload(Handle lib) {
    FreeLibrary((HMODULE)lib.val[0]);
}
