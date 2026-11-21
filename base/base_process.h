#if !defined(BASE_PROCESS_H)
#define BASE_PROCESS_H

typedef U8 ProcessPriority;

enum {
    PROCESS_PRIORITY_NORMAL    = 0,
    PROCESS_PRIORITY_IDLE      = bit(0),
    PROCESS_PRIORITY_HIGH      = bit(1),
    PROCESS_PRIORITY_REALTIME  = bit(2),
};

typedef struct ProcessCreateParams ProcessCreateParams;
struct ProcessCreateParams {
    Str8 cmd_line;
    Str8 stdin_path;
    Str8 stdout_path;
    Str8 stderr_path;
    ProcessPriority priority;
    FileAccessFlag stdout_access_flags;
    FileAccessFlag stdin_access_flags;
    FileAccessFlag stderr_access_flags;
    U8 inherit_handles;
    U8 run_detached;
    U8 hidden;
    U8 wait;
    U8 strip_default_header;
};

internal void process_cli_attach(U32 if_exists);

internal U32 process_create_(ProcessCreateParams* params);

#define process_create(...) process_create_(&(ProcessCreateParams){.cmd_line = S("help"), __VA_ARGS__})

/* Cmd interface */

typedef U8 CmdFlag;

enum {
    CMD_FLAG_NONE,
    CMD_FLAG_INHERIT_HANDLES    = bit(0),
    CMD_FLAG_RUN_DETACHED       = bit(1),
    CMD_FLAG_HIDDEN             = bit(2),
    CMD_FLAG_WAIT               = bit(3),
};

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
internal U32 cmd_run(Cmd* cmds, CmdFlag flags);

#endif /* BASE_PROCESS_H */