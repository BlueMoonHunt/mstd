#if !defined(BASE_FILE_H)
#define BASE_FILE_H

typedef enum FileAccessFlag {
    FILE_ACCESS_FLAG_OPEN_EXISTING  = 1 << 0,
    FILE_ACCESS_FLAG_OPEN_ALWAYS    = 1 << 1,
    FILE_ACCESS_FLAG_SHARE_READ     = 1 << 2,
    FILE_ACCESS_FLAG_SHARE_WRITE    = 1 << 3,
    FILE_ACCESS_FLAG_EXECUTE        = 1 << 5,
    FILE_ACCESS_FLAG_READ           = 1 << 6,
    FILE_ACCESS_FLAG_WRITE          = 1 << 7,
    FILE_ACCESS_FLAG_APPEND         = 1 << 8
} FileAccessFlag;

#define FILE_MAX_RETRIES 5

internal U32 file_delete(Str8 path);
internal U32 file_copy(Str8 src, Str8 dest);
internal U32 file_move(Str8 src, Str8 dest);
internal U32 file_exists(Str8 path);
internal U32 file_directory_exists(Str8 path);
internal U64 file_size_get(Handle file);

internal Handle file_open(Str8 name, FileAccessFlag flags);
internal void file_close(Handle file);
internal void file_write(Handle file, void* data, U64 size, U64 offset);
internal Str8 file_read(Arena* arena, Handle file, U64 size, U64 offset);

#define file_read_struct(arena, file, T, offset) (T*)os_file_read(arena, file, sizeof(T), offset)
#define file_write_struct(file, struct_ptr, offset) os_file_write(file, struct_ptr, offset, sizeof(*struct_ptr))
#define file_write_string(file, str) os_file_write(file, str.data, 0, str.size)

/* FileWatcher */

typedef U8 FileEventType;

enum {
    FILE_EVENT_TYPE_NULL,
    FILE_EVENT_TYPE_MODIFIED,
    FILE_EVENT_TYPE_ADDED,
    FILE_EVENT_TYPE_DELETED,
    FILE_EVENT_TYPE_RENAMED,
};

typedef struct FileEvents {
    U64 count;
    FileEventType* type_array;
    Str8* file_array;
} FileEvents;

internal Handle file_watcher_create(Arena* arena, Str8 path, U32 watch_sub_directory);
internal void file_watcher_poll_events(Arena* arena, Handle watcher, FileEvents* events, U32 timeout_ms);
internal void file_watcher_destroy(Handle watcher);

#endif /* BASE_FILE_H */