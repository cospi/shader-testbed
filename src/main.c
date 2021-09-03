#include <stdbool.h>

#include <Windows.h>

#include "testbed.h"
#include "wgl.h"

static bool s_running = true;
static UINT s_width = 0;
static UINT s_height = 0;
static bool s_reload_testbed = false;

static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch (message) {
    case WM_CLOSE:
        s_running = false;
        return 0;
    case WM_SIZE:
        s_width = LOWORD(l_param);
        s_height = HIWORD(l_param);
        break;
    case WM_KEYDOWN:
        if (w_param == VK_F5) {
            s_reload_testbed = true;
        }
        break;
    default:
        break;
    }

    return DefWindowProcW(window, message, w_param, l_param);
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

    if (!wgl_init_context_creation_extensions(instance)) {
        show_error(L"Initializing OpenGL context creation extensions failed.");
        return -1;
    }

    wchar_t window_class_name[] = L"ShaderTestbedWindowClass";

    WNDCLASSEXW window_class = {
        .cbSize = sizeof(WNDCLASSEXW),
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
        WS_OVERLAPPEDWINDOW,
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

    HDC device_context = GetDC(window);
    if (device_context == NULL) {
        show_error(L"GetDC() failed.");
        return -1;
    }

    HGLRC rendering_context = wgl_create_context(device_context);
    if (rendering_context == NULL) {
        show_error(L"Creating OpenGL rendering context failed.");
        return -1;
    }

    if (wglMakeCurrent(device_context, rendering_context) == FALSE) {
        show_error(L"wglMakeCurrent() failed.");
        return -1;
    }

    if (!wgl_init_extensions()) {
        show_error(L"Initializing OpenGL extensions failed.");
        return -1;
    }

    RECT window_rect;
    if (GetWindowRect(window, &window_rect) == FALSE) {
        show_error(L"GetWindowRect() failed.");
        return -1;
    }

    s_width = (UINT)(window_rect.right - window_rect.left);
    s_height = (UINT)(window_rect.bottom - window_rect.top);

    testbed_init();

    ShowWindow(window, SW_SHOW);

    long double time = 0.0L;
    LARGE_INTEGER query_performance_result;
    // QueryPerformaceFrequency and QueryPerformanceCounter never fail on Windows XP or later.
    long double performance_frequency;
    QueryPerformanceFrequency(&query_performance_result);
    performance_frequency = (long double)query_performance_result.QuadPart;
    LONGLONG previous_performance_counter;
    QueryPerformanceCounter(&query_performance_result);
    previous_performance_counter = query_performance_result.QuadPart;

    while (s_running) {
        MSG message;
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE) != 0) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        if (s_reload_testbed) {
            s_reload_testbed = false;
            testbed_reload();
        }

        QueryPerformanceCounter(&query_performance_result);
        LONGLONG performance_counter = query_performance_result.QuadPart;
        long double delta_time = (performance_counter - previous_performance_counter) / performance_frequency;
        time += delta_time;
        testbed_update((GLsizei)s_width, (GLsizei)s_height, (float)time, (float)delta_time);
        previous_performance_counter = performance_counter;

        SwapBuffers(device_context);
    }

    return 0;
}
