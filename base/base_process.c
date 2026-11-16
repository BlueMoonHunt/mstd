internal Cmd* cmd_create(void) {
    Arena* arena;
    Cmd* cmds;

    arena   = arena_alloc(.flags = ARENA_FLAG_INTERNAL);
    cmds    = arena_push_type(arena, Cmd);

    cmds->arena = arena;
    cmds->head  = 0;
    cmds->tail  = 0;

    return cmds;
}

internal void cmd_append(Cmd* cmds, Str8 cmd_str) {
    CmdNode* node;

    node = arena_push_type(cmds->arena, CmdNode);
    node->data = cmd_str;

    sll_queue_push(cmds->head, cmds->tail, node);
}

internal void cmd_reset(Cmd* cmds) {
    arena_reset(cmds->arena);

    cmds->head = 0;
    cmds->tail = 0;
}

internal void cmd_destroy(Cmd* cmds) {
    arena_release(cmds->arena);
    cmds->head = 0;
    cmds->tail = 0;
}

internal U32 cmd_run(Cmd* cmds, ProcessFlag flags) {
    CmdNode* node;
    Str8 cmd_str;

    cmd_str.size = 0;

    for_each_node(node, cmds) {
        cmd_str.size += node->data.size;
    }

    cmd_str = str8_from_size(cmds->arena, cmd_str.size);
    cmd_str.size = 0;

    for_each_node(node, cmds) {
        mem_copy(cmd_str.data + cmd_str.size, node->data.data, node->data.size);
        cmd_str.size += node->data.size;
    }

    return process_create(.cmd_line           = cmd_str,
                          .inherit_handles    = flags & PROCESS_FLAG_INHERIT_HANDLES,
                          .run_detached       = flags & PROCESS_FLAG_RUN_DETACHED,
                          .wait               = flags & PROCESS_FLAG_WAIT,
                          .hidden             = flags & PROCESS_FLAG_HIDDEN);
}