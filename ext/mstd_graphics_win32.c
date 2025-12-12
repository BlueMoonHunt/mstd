#include <mstd_graphics.h>
#include <Windows.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

typedef struct Window Window;
struct Window {
    HWND handle;
    str16 title;
};

LRESULT CALLBACK main_window_callback(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
    case WM_PAINT: {
        PAINTSTRUCT paint;
        HDC device_context = BeginPaint(window_handle, &paint);
        paint.rcPaint;

        PatBlt(device_context,
            paint.rcPaint.left,
            paint.rcPaint.top,
            paint.rcPaint.right - paint.rcPaint.left,
            paint.rcPaint.bottom - paint.rcPaint.top,
            WHITENESS
        );
    } break;
    case WM_SIZE: {} break;
    case WM_CLOSE: {
        PostQuitMessage(0);
    } break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(window_handle, message, wparam, lparam);
    }

    return 0;
}

Window* window_create(WindowCreateInfo create_info, Arena* arena) {
    Window* window = arena_push_struct(arena, Window);
    WNDCLASSEX window_class = {0};
    HINSTANCE instance = GetModuleHandle(NULL);

    window->title = str16_of_size(create_info.title.size, arena);
    mbstowcs(window->title.str, create_info.title.str, window->title.size);

    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = main_window_callback;
    window_class.hInstance = instance;
    window_class.lpszClassName = window->title.str;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassEx(&window_class))
        return 0;

    HWND window_handle = CreateWindowEx(
        0,
        window->title.str,
        L"My Application",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        (create_info.pos.x > 0) ? create_info.pos.x : CW_USEDEFAULT, (create_info.pos.y > 0) ? create_info.pos.y : CW_USEDEFAULT,
        (create_info.size.width > 0) ? create_info.size.width : CW_USEDEFAULT, (create_info.size.height > 0) ? create_info.size.height : CW_USEDEFAULT,
        0, 0,
        instance,
        0
    );

    if (!window_handle)
        return NULL;

    window->handle = window_handle;
    return window;
}