#include <mstd.c>

int mstd_main(int argc, char* argv[]); // To be declared by user

#if defined(MSTD_USE_GUI_ENTRYPOINT)
    #include "mstd_graphics.c"
#endif

#if defined(OS_WINDOWS)
#define NOMINMAX
#include <Windows.h>
#include <stdio.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    unreferenced_parameter(hInstance);
    unreferenced_parameter(hPrevInstance);
    unreferenced_parameter(lpCmdLine);
    unreferenced_parameter(nCmdShow);

    BOOL attached = AttachConsole(ATTACH_PARENT_PROCESS);

    if (attached) {
        FILE *fp;

        freopen_s(&fp, "CONIN$",  "r", stdin);
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
    }

    int result = mstd_main(__argc, __argv);
    FreeConsole();
    return result;
}
#else
int main(int argc, char* argv[]) {
    return mstd_main(argc, argv);
}
#endif