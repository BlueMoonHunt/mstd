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

internal U32 os_cmd_opt(char* command, CmdOpt opt) {
    Arena* arena = arena_scratch_begin();
    Str8 cmd_str = str8_concat(arena, str8_lit("powershell -Command "), str8(command));

    STARTUPINFOA si = {0};
    si.cb = sizeof(si);

    HANDLE nul = INVALID_HANDLE_VALUE;

    if (!opt.hidden && GetConsoleWindow()) {
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        SECURITY_ATTRIBUTES sa = {
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = NULL,
            .bInheritHandle = TRUE,
        };
        nul = CreateFileA("NUL", GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, NULL);

        if (nul != INVALID_HANDLE_VALUE) {
            si.dwFlags |= STARTF_USESTDHANDLES;
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
            si.hStdOutput = nul;
            si.hStdError = nul;
        }
    }

    DWORD flags = NORMAL_PRIORITY_CLASS;
    switch ((CmdPiority)opt.priority) {
    case CMD_PRIORITY_IDLE:
        flags = IDLE_PRIORITY_CLASS;
        break;
    case CMD_PRIORITY_HIGH:
        flags = HIGH_PRIORITY_CLASS;
        break;
    case CMD_PRIORITY_REALTIME:
        flags = REALTIME_PRIORITY_CLASS;
        break;
    default:
        break;
    }

    if (opt.hidden || !GetConsoleWindow())
        flags |= CREATE_NO_WINDOW;

    if (opt.run_ditached) {
        flags &= ~CREATE_NO_WINDOW;
        flags |= DETACHED_PROCESS;
    }

    BOOL inherit = (si.dwFlags & STARTF_USESTDHANDLES) ? TRUE : (BOOL)opt.inherit_handles;

    PROCESS_INFORMATION pi = {0};
    BOOL ok = CreateProcessA(NULL, (char *)cmd_str.data, NULL, NULL, inherit, flags, NULL, NULL, &si, &pi);

    if (nul != INVALID_HANDLE_VALUE)
        CloseHandle(nul);

    U32 result = 1;

    if (!opt.run_ditached)
        WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD code = 0;
    if(!ok || (GetExitCodeProcess(pi.hProcess, &code) && code != 0))
        result = 0;

    arena_scratch_end(arena);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return result;
}