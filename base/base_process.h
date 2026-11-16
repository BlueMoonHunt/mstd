#if !defined(BASE_PROCESS_H)
#define BASE_PROCESS_H

typedef U32 ProcessPriority;

enum {
    PROCESS_PRIORITY_NORMAL    = 0,
    PROCESS_PRIORITY_IDLE      = bit(0),
    PROCESS_PRIORITY_HIGH      = bit(1),
    PROCESS_PRIORITY_REALTIME  = bit(2),
};

typedef U8 ProcessFlag;

enum {
    PROCESS_FLAG_NONE,
    PROCESS_FLAG_INHERIT_HANDLES    = bit(0),
    PROCESS_FLAG_RUN_DETACHED       = bit(1),
    PROCESS_FLAG_HIDDEN             = bit(2),
    PROCESS_FLAG_WAIT               = bit(3),
};

typedef struct ProcessCreateParams ProcessCreateParams;
struct ProcessCreateParams {
    Str8 cmd_line;
    Str8 cmd_header;
    Str8 stdin_path;
    Str8 stdout_path;
    Str8 stderr_path;
    ProcessPriority priority;
    FileAccessFlag stdout_access_flags;
    FileAccessFlag stdin_access_flags;
    FileAccessFlag stderr_access_flags;

    union {
        ProcessFlag flags;
        struct {
            U8 inherit_handles  : 2;
            U8 run_detached     : 2;
            U8 hidden           : 2;
            U8 wait             : 2;
        };
    };
};

internal void process_cli_attach(U32 if_exists);

internal U32 process_create_(ProcessCreateParams* params);

#define process_create(...) process_create_(&(ProcessCreateParams){.cmd_line = str8_lit(""), .flags = PROCESS_FLAG_NONE, __VA_ARGS__})

/* Cmd interface */

typedef struct CmdNode CmdNode;
struct CmdNode {
    Str8 data;
    CmdNode* next;
};

typedef struct Cmd Cmd;
struct Cmd {
    Arena* arena;
    CmdNode* head;
    CmdNode* tail;
};

internal Cmd* cmd_create(void);
internal void cmd_append(Cmd* cmds, Str8 cmd_str);
internal void cmd_reset(Cmd* cmds);
internal void cmd_destroy(Cmd* cmds); /* releases the internal arena */
internal U32 cmd_run(Cmd* cmds, ProcessFlag flags);

#endif /* BASE_PROCESS_H */
