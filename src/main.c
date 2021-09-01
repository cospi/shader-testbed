#include <stdbool.h>

#include <Windows.h>
#include <gl/GL.h>

static bool s_running = true;
static UINT s_width = 0;
static UINT s_height = 0;

static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch (message) {
    case WM_SIZE:
        s_width = LOWORD(l_param);
        s_height = HIWORD(l_param);
        return 0;
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

    HDC device_context = GetDC(window);
    if (device_context == NULL) {
        show_error(L"GetDC() failed.");
        return -1;
    }

    PIXELFORMATDESCRIPTOR pixel_format = {
        .nSize = sizeof(PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        // MSDN says cColorBits should exclude the alpha bitplanes, but debugging suggests otherwise.
        .cColorBits = 32,
        // For most bits and shifts, just accept what the OS gives back.
        .cRedBits = 0,
        .cRedShift = 0,
        .cGreenBits = 0,
        .cGreenShift = 0,
        .cBlueBits = 0,
        .cBlueShift = 0,
        .cAlphaBits = 0,
        .cAlphaShift = 0,
        .cAccumBits = 0,
        .cAccumRedBits = 0,
        .cAccumGreenBits = 0,
        .cAccumBlueBits = 0,
        .cAccumAlphaBits = 0,
        .cDepthBits = 24,
        .cStencilBits = 0,
        .cAuxBuffers = 0,
        .iLayerType = 0,
        .bReserved = 0,
        .dwLayerMask = 0,
        .dwVisibleMask = 0,
        .dwDamageMask = 0
    };
    int suggested_pixel_format_index = ChoosePixelFormat(device_context, &pixel_format);
    if (suggested_pixel_format_index == 0) {
        show_error(L"ChoosePixelFormat() failed.");
        return -1;
    }

    if (DescribePixelFormat(
        device_context,
        suggested_pixel_format_index,
        sizeof(PIXELFORMATDESCRIPTOR),
        &pixel_format
    ) == 0) {
        show_error(L"DescribePixelFormat() failed.");
        return -1;
    }

    if (SetPixelFormat(device_context, suggested_pixel_format_index, &pixel_format) == FALSE) {
        show_error(L"SetPixelFormat() failed.");
        return -1;
    }

    HGLRC rendering_context = wglCreateContext(device_context);
    if (rendering_context == NULL) {
        show_error(L"wglCreateContext() failed.");
        return -1;
    }

    if (wglMakeCurrent(device_context, rendering_context) == FALSE) {
        show_error(L"wglMakeCurrent() failed.");
        return -1;
    }

    RECT window_rect;
    if (GetWindowRect(window, &window_rect) == FALSE) {
        show_error(L"GetWindowRect() failed.");
        return -1;
    }
    
    s_width = (UINT)(window_rect.right - window_rect.left);
    s_height = (UINT)(window_rect.bottom - window_rect.top);

    while (s_running) {
        MSG message;
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE) != 0) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        glViewport(0, 0, (GLsizei)s_width, (GLsizei)s_height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-0.5f, -0.5f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.5f, -0.5f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.5f, 0.0f);
        glEnd();

        SwapBuffers(device_context);
    }

    return 0;
}
