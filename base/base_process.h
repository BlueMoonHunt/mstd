#if !defined(BASE_PROCESS_H)
#define BASE_PROCESS_H

typedef U32 ProcessPriority;

enum {
    PROCESS_PRIORITY_NORMAL    = 0,
    PROCESS_PRIORITY_IDLE      = bit(0),
    PROCESS_PRIORITY_HIGH      = bit(1),
    PROCESS_PRIORITY_REALTIME  = bit(2),
};

typedef struct ProcessCreateParams ProcessCreateParams;
struct ProcessCreateParams {
    Str8 cmd_line;
    Str8 cmd_header;                /* optional : use in case you need to run a custom shell. */
    ProcessPriority priority;
    U8 inherit_handles;             /* other than setting bool it also attaches parent's STD_IN, STD_OUT and STD_ERROR */
    U8 run_detached;
    U8 hidden;
};

internal void process_cli_attach(U32 if_exists);

internal U32 process_create_(ProcessCreateParams* params);

#define process_create(cmd_str, ...) process_create_(&(ProcessCreateParams){.cmd_line = cmd_str, __VA_ARGS__})

#endif /* BASE_PROCESS_H */