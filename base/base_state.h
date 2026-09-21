#if !defined(BASE_STATE_H)
#define BASE_STATE_H

typedef struct SystemInfo SystemInfo;
struct SystemInfo {
    U32 page_size;
    U32 logical_processor_count;
    U32 allocation_granularity;
    U64 large_page_size;
    U64 microsecond_resolution;
};

typedef struct ProcessInfo ProcessInfo;
struct ProcessInfo {
    U32 id;
    Str8 bin_path;
    Str8 current_working_directory;
};

internal SystemInfo* system_info(void);
internal ProcessInfo* process_info(void);

internal void os_state_init(void);

internal Handle lib_load(char* lib_name);
internal void* lib_symbol_get(Handle lib, const char* symbol_name_cstr);
internal void lib_unload(Handle lib);

internal force_inline void cpu_relax(void);

#endif /* BASE_STATE_H */