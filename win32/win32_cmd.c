internal void os_cli_alloc(U32 if_not_exists) {
    if ((if_not_exists) ? AllocConsole() : AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE *dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
        freopen_s(&dummy, "CONIN$", "r", stdin);

        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);
    }
}

internal void os_cmd_line_args(Arena* arena, I32* argc, Str8** argv) {
    I32 count, i;
    LPWSTR* args;

    count = 0;
    args = CommandLineToArgvW(GetCommandLineW(), &count);

    *argv = arena_push_array(arena, Str8, count);
    *argc = count;

    for (i = 0; i < count; i++) {
        *argv[i] = str8_from_16(arena, str16(args[i]));
    }

    LocalFree(args);
}

internal U32 os_cmd(char* command, enum_val(CmdFlag, U8) flags) {
    DWORD priority;
    Arena* arena;
    Str8 cmd_str;
    STARTUPINFOA si;
    HANDLE nul;
    SECURITY_ATTRIBUTES sa;
    BOOL inherit;
    PROCESS_INFORMATION pi;
    BOOL ok;
    U32 result;
    DWORD code;

    arena = arena_scratch_begin();
    cmd_str = str8_concat(arena, str8_lit("powershell -Command "), str8(command));

    si.cb = sizeof(si);

    nul = INVALID_HANDLE_VALUE;

    if (!(flags & CMD_FLAG_HIDDEN) && GetConsoleWindow()) {
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;

        nul = CreateFileA("NUL", GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, NULL);

        if (nul != INVALID_HANDLE_VALUE) {
            si.dwFlags |= STARTF_USESTDHANDLES;
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
            si.hStdOutput = nul;
            si.hStdError = nul;
        }
    }

    priority = NORMAL_PRIORITY_CLASS;

    if (flags & CMD_FLAG_PRIORITY_HIGH)             priority = HIGH_PRIORITY_CLASS;
    else if (flags & CMD_FLAG_PRIORITY_IDLE)        priority = IDLE_PRIORITY_CLASS;
    else if (flags & CMD_FLAG_PRIORITY_REALTIME)    priority = REALTIME_PRIORITY_CLASS;

    if ((flags & CMD_FLAG_HIDDEN) || !GetConsoleWindow())
        flags |= CREATE_NO_WINDOW;

    if ((flags & CMD_FLAG_RUN_DITACHED)) {
        flags &= ~CREATE_NO_WINDOW;
        flags |= DETACHED_PROCESS;
    }

    inherit = (si.dwFlags & STARTF_USESTDHANDLES) ? TRUE : (BOOL)(flags & CMD_FLAG_INHERIT_HANDLE);

    ok = CreateProcessA(NULL, (char *)cmd_str.data, NULL, NULL, inherit, priority, NULL, NULL, &si, &pi);

    if (nul != INVALID_HANDLE_VALUE)
        CloseHandle(nul);

    result = 1;

    if (!(flags & CMD_FLAG_RUN_DITACHED))
        WaitForSingleObject(pi.hProcess, INFINITE);

    if(!ok || (GetExitCodeProcess(pi.hProcess, &code) && code != 0))
        result = 0;

    arena_scratch_end(arena);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return result;
}