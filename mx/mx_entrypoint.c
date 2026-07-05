#pragma comment(lib, "gdi32")
#pragma comment(lib, "winmm")
#pragma comment(lib, "comctl32")
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") /* this is required for loading correct comctl32 dll file */

void entrypoint(I32 argc, Str8* argv); /* To be defined by user */

internal void win32_entrypoint_caller(I32 argc, WCHAR** wargv) {
    I32 i;
    Str8* argv;

    win32_state_init();

    argv = arena_push_array(win32_state.arena, Str8, argc);

    for (i = 0; i < argc; i++) {
        Str16 arg16 = str16_from_cstr(wargv[i]);
        argv[i] = str8_from_16(win32_state.arena, arg16);
    }

    entrypoint(argc, argv);
}

#if ! defined(BUILD_GRAPHICAL_INTERFACE)

int wmain(int argc, WCHAR** wargv) {
    win32_entrypoint_caller(argc, wargv);
    return 0;
}

#else

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nShowCmd;

    CoInitializeEx(0, COINIT_APARTMENTTHREADED);
    win32_entrypoint_caller(__argc, __wargv);
    return 0;
}

#endif