#include <stdbool.h>

#include <Windows.h>

static bool s_running = true;

static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch (message) {
    case WM_CLOSE:
        s_running = false;
        return 0;
    default:
        return DefWindowProcW(window, message, w_param, l_param);
    }
}

static void show_error(LPCWSTR error)
{
    MessageBoxExW(NULL, error, NULL, MB_OK | MB_ICONERROR, 0);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous_instance, LPSTR cmd_line, int cmd_show)
{
    // Resource cleanup is left to the OS.

    (void)previous_instance;
    (void)cmd_line;
    (void)cmd_show;

    const wchar_t window_class_name[] = L"ShaderTestbedWindowClass";

    WNDCLASSEXW window_class = {
        .cbSize = sizeof(WNDCLASSEXA),
        .style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC,
        .lpfnWndProc = window_proc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = instance,
        .hIcon = NULL,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = NULL,
        .lpszMenuName = NULL,
        .lpszClassName = window_class_name,
        .hIconSm = NULL
    };
    if (RegisterClassExW(&window_class) == 0) {
        show_error(L"RegisterClassExW() failed.");
        return -1;
    }

    HWND window = CreateWindowExW(
        0,
        window_class_name,
        L"Shader Testbed",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        instance,
        NULL
    );
    if (window == NULL) {
        show_error(L"CreateWindowExW() failed.");
        return -1;
    }

    while (s_running) {
        MSG message;
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE) != 0) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    return 0;
}
