internal U32 os_file_delete(Str8 path) {
    I32 i;
    I32 sleep_ms;
    DWORD err;

    for (i = 0, sleep_ms = 1; i < OS_FILE_OP_MAX_RETRIES; i++) {
        if (DeleteFileA((char *)path.data))
            return 1;

        err = GetLastError();
        if (err != ERROR_SHARING_VIOLATION && err != ERROR_LOCK_VIOLATION)
            break;

        os_thread_sleep(sleep_ms);
        sleep_ms *= 2;
    }

    return 0;
}

internal U32 os_file_copy(Str8 src, Str8 dest) {
    I32 sleep_ms;
    I32 i;

    for (i = 0, sleep_ms = 1; i < OS_FILE_OP_MAX_RETRIES; i++) {
        if (CopyFileExA((char *)src.data, (char *)dest.data, NULL, NULL, FALSE, 0))
            return 1;

        DWORD err = GetLastError();
        if (err != ERROR_SHARING_VIOLATION && err != ERROR_ACCESS_DENIED && err != ERROR_LOCK_VIOLATION)
            break;

        os_thread_sleep(sleep_ms);
        sleep_ms *= 2;
    }

    return 0;
}

internal U32 os_file_move(Str8 src, Str8 dest) {
    I32 i;
    I32 sleep_ms;

    for (i = 0, sleep_ms = 1; i < OS_FILE_OP_MAX_RETRIES; i++) {
        if (MoveFileExA((char*)src.data, (char*)dest.data, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH)) {
            return 1;
        }

        DWORD err = GetLastError();
        if (err != ERROR_SHARING_VIOLATION && err != ERROR_LOCK_VIOLATION)
            break;

        os_thread_sleep(sleep_ms);
        sleep_ms *= 2;
    }

    return 0;
}

internal U32 os_file_exists(Str8 path) {
    DWORD attributes;
    U32 exists;

    attributes = GetFileAttributesA((char *)path.data);
    exists = (attributes != INVALID_FILE_ATTRIBUTES) && !!(~attributes & FILE_ATTRIBUTE_DIRECTORY);

    return exists;
}

internal U32 os_file_directory_exists(Str8 path) {
    DWORD attributes;
    U32 exists;

    attributes = GetFileAttributesA((char *)path.data);
    exists = (attributes != INVALID_FILE_ATTRIBUTES) && (attributes & FILE_ATTRIBUTE_DIRECTORY);

    return exists;
}

internal Handle os_file_open(Str8 name, FileAccessFlag flags) {
    Handle result = {0};
    Arena *arena = arena_scratch_begin();
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

    HANDLE file = CreateFileW((WCHAR *)path_w32.data, access_flags, share_mode, 0, creation_disposition,
                              FILE_ATTRIBUTE_NORMAL, 0);
    if (file != INVALID_HANDLE_VALUE)
        result.val[0] = (U64)file;
    arena_scratch_end(arena);

    return result;
}

internal U64 os_file_size(Handle handle) {
    U64 size = 0;
    GetFileSizeEx((HANDLE)handle.val[0], (LARGE_INTEGER *)&size);
    return size;
}

internal void os_file_close(Handle handle) { CloseHandle((HANDLE)handle.val[0]); }

internal Str8 os_file_read(Arena *arena, Handle handle, U64 offset, U64 size) {
    U64 total_read_size = 0;

    Str8 str;

    str.data = 0;
    str.size = 0;

    if (handle.val[0]) {
        HANDLE file = (HANDLE)handle.val[0];
        U64 _size = 0;
        GetFileSizeEx(file, (LARGE_INTEGER *)&_size);

        U64 total_to_read = clamp_top(size, _size);
        U8 *out = arena_push_array(arena, U8, total_to_read + 1);

        for (U64 _offset = offset; total_read_size < total_to_read;) {
            U64 remaining = total_to_read - total_read_size;
            DWORD to_read = (DWORD)clamp_top(remaining, u32_max);
            DWORD actual_read = 0;

            OVERLAPPED overlapped = {0};
            overlapped.Offset = (DWORD)(_offset & u32_max);
            overlapped.OffsetHigh = (DWORD)(_offset >> 32);

            if (ReadFile(file, (U8 *)out + total_read_size, to_read, &actual_read, &overlapped)) {
                _offset += actual_read;
                total_read_size += actual_read;
                if (actual_read < to_read)
                    break;
            } else
                break;
        }
        out[total_read_size] = 0;

        str.size = total_read_size;
        str.data = out;
    }

    return str;
}

internal void os_file_write(Handle handle, void *data, U64 offset, U64 size) {
    U64 total_written = 0;

    if (handle.val[0]) {
        for (U64 dest_offset = offset;;) {

            U64 remaining = size - total_written;
            if (remaining == 0)
                break;

            DWORD to_write = (DWORD)clamp_top(remaining, MB(1));
            DWORD actual_write = 0;

            OVERLAPPED overlapped = {0};
            overlapped.Offset = (DWORD)(dest_offset & 0xFFFFFFFF);
            overlapped.OffsetHigh = (DWORD)(dest_offset >> 32);

            if (!WriteFile((HANDLE)handle.val[0], (U8 *)data + total_written, to_write, &actual_write, &overlapped))
                break;
            total_written += actual_write;
            dest_offset += actual_write;

            if (remaining == 0)
                break;
        }
    }
}

internal FileWatcher *os_file_watcher_create(Arena *arena, Str8 path, U32 watch_sub_directory) {
    Arena *scratch = arena_scratch_begin();
    Str16 u16_path = str16_from_8(scratch, path);

    HANDLE dir =
        CreateFileW((LPCWSTR)u16_path.data, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

    struct Win32FileWatcher watcher = {0};

    if (dir == INVALID_HANDLE_VALUE)
        return NULL;

    watcher.scan_sub_directories = watch_sub_directory;
    watcher.dir_handle = dir;
    watcher.iocp = CreateIoCompletionPort(watcher.dir_handle, NULL, 0, 1);

    ReadDirectoryChangesW(watcher.dir_handle, watcher.notification_buffer, sizeof(watcher.notification_buffer),
                          watcher.scan_sub_directories, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
                          NULL, &watcher.overlapped, NULL);

    arena_scratch_end(scratch);
    FileWatcher *result = (FileWatcher *)arena_push_type(arena, struct Win32FileWatcher);
    return (result);
}

internal FileEvent *os_file_watcher_poll_events(FileWatcher *watcher, Arena *arena, U32 timeout_ms, U32 *out_count) {
    DWORD bytes = 0;
    ULONG_PTR key = 0;
    LPOVERLAPPED p_overlapped = NULL;

    FileEvent *result_array = NULL;
    U32 count = 0;

    if (GetQueuedCompletionStatus(watcher->iocp, &bytes, &key, &p_overlapped, (DWORD)timeout_ms)) {
        if (p_overlapped == &watcher->overlapped && bytes > 0) {

            FILE_NOTIFY_INFORMATION *notify = (FILE_NOTIFY_INFORMATION *)watcher->notification_buffer;
            for (;;) {
                count++;
                if (notify->NextEntryOffset == 0)
                    break;
                notify = (FILE_NOTIFY_INFORMATION *)((U8 *)notify + notify->NextEntryOffset);
            }

            result_array = arena_push_array(arena, FileEvent, count);

            notify = (FILE_NOTIFY_INFORMATION *)watcher->notification_buffer;

            for (U32 i = 0; i < count; i++) {
                U32 name_len_chars = notify->FileNameLength / sizeof(U16);
                Str16 u16_file = {(U16 *)notify->FileName, name_len_chars};

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
                notify = (FILE_NOTIFY_INFORMATION *)((U8 *)notify + notify->NextEntryOffset);
            }
        }

        mem_zero_struct(&watcher->overlapped);
        ReadDirectoryChangesW(watcher->dir_handle, watcher->notification_buffer, sizeof(watcher->notification_buffer),
                              watcher->scan_sub_directories,
                              FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME |
                                  FILE_NOTIFY_CHANGE_DIR_NAME,
                              NULL, &watcher->overlapped, NULL);
    }

    *out_count = count;
    return result_array;
}

internal void os_file_watcher_destroy(FileWatcher *watcher) {
    CloseHandle(watcher->iocp);
    CloseHandle(watcher->dir_handle);
}