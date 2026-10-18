LRESULT CALLBACK w32_message_callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}