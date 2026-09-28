#if !defined(W32_BASE_H)
#define W32_BASE_H

#include <Windows.h>
#include <shellapi.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "shell32")

#define W32_WINDOW_CLASS L"W32_WINDOW_CLASS"
#define W32_FILE_WATCHER_NOTIFICATION_BUFFER_SIZE KB(8)

typedef struct W32_FileWatcher W32_FileWatcher;
struct W32_FileWatcher {
    HANDLE dir_handle;
    HANDLE iocp;
    U8 notification_buffer[W32_FILE_WATCHER_NOTIFICATION_BUFFER_SIZE];
    OVERLAPPED overlapped;
    U32 scan_sub_directories;
};

typedef U8 W32_EntityType;

enum {
    W32_ENTITY_TYPE_UNKNOWN,
    W32_ENTITY_TYPE_THREAD,
    W32_ENTITY_TYPE_MUTEX,
    W32_ENTITY_TYPE_RWMUTEX,
    W32_ENTITY_TYPE_COND_VAR,
    W32_ENTITY_TYPE_BARRIER,
    W32_ENTITY_TYPE_COUNT
};

typedef struct W32_Entity W32_Entity;
struct W32_Entity {
    W32_Entity* next;
    W32_EntityType type;
    union {
        struct
        {
            ThreadEntryFunction* func;
            void* data;
            HANDLE handle;
            DWORD id;
        } thread;
        CRITICAL_SECTION mutex;
        SRWLOCK rw_mutex;
        CONDITION_VARIABLE cond_var;
        SYNCHRONIZATION_BARRIER synchronization_barrier;
    };
};

typedef struct W32_State W32_State;
struct W32_State {
    Arena* arena;

    SystemInfo system;
    ProcessInfo process;

    CRITICAL_SECTION entity_mutex;
    W32_Entity* entity_stack_head;
    W32_Entity* entity_stack_tail;
};

global W32_State w32_state;
global thread_var HANDLE w32_thread_timer;

internal W32_Entity* w32_entity_alloc(W32_EntityType type);
internal void w32_entity_release(W32_Entity* entity);

internal DWORD w32_thread_entrypoint(void* ptr);

internal U32 w32_sleep_ms_from_end_us(U64 end_us);

internal void w32_class_create(void);

#endif /* W32_BASE_H */