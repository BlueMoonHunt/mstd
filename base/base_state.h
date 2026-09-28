#if !defined(BASE_STATE_H)
#define BASE_STATE_H


typedef U8 ReleaseType;

enum {
    RELEASE_FLAG_ALPHA,
    RELEASE_FLAG_BETA,
    RELEASE_FLAG_RC,
    RELEASE_FLAG_STABLE,
};

typedef struct Version Version;
struct Version {
    U8 major;
    U8 minor;
    U8 patch;
    ReleaseType type;
};

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

typedef U32 OS_StateFlag;

enum {
    OS_STATE_FLAG_NONE,
    OS_STATE_FLAG_VIDEO,
};

typedef struct OS_StateParam OS_StateParam;
struct OS_StateParam {
    OS_StateFlag flags;
};

internal SystemInfo* system_info(void);
internal ProcessInfo* process_info(void);

internal void os_state_init_(OS_StateParam* params);
#define os_state_init(...) os_state_init_(&(OS_StateParam){.flags = 0, __VA_ARGS__})

internal Handle lib_load_(Str8 lib_name);
internal void* lib_symbol_get_(Handle lib, Str8 symbol_name);
internal void lib_unload(Handle lib);

#define lib_load(lib_name) lib_load_(str8_lit(lib_name))
#define lib_symbol_get(lib, symbol_name) lib_symbol_get_(lib, str8_lit(symbol_name))

internal force_inline void cpu_relax(void);

#endif /* BASE_STATE_H */