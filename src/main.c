#include <stdbool.h>

#include <Windows.h>

#include "gl.h"
#include "testbed.h"

#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_ALPHA_BITS_ARB 0x201B
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_TYPE_RGBA_ARB 0x202B
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC, const int *, const float *, UINT, int *, UINT *);
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int *);

static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

static bool s_running = true;
static UINT s_width = 0;
static UINT s_height = 0;
static bool s_reload_testbed = false;

// SetPixelFormat can only be called once on a device context.
// Therefore, a dummy device context is necessary for initializing the context creation extensions.
static bool init_gl_context_creation_extensions(HINSTANCE instance)
{
    wchar_t window_class_name[] = L"ShaderTestbedWGLExtensionsWindowClass";
    WNDCLASSEXW window_class = {
        .cbSize = sizeof(WNDCLASSEXW),
        .style = CS_OWNDC,
        .lpfnWndProc = DefWindowProcA,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = instance,
        .hIcon = NULL,
        .hCursor = NULL,
        .hbrBackground = NULL,
        .lpszMenuName = NULL,
        .lpszClassName = window_class_name,
        .hIconSm = NULL
    };
    PIXELFORMATDESCRIPTOR pixel_format = {
        .nSize = sizeof(PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags = PFD_SUPPORT_OPENGL,
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
    bool success = false;
    HWND window;
    HDC device_context;
    int suggested_pixel_format_index;
    HGLRC rendering_context;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wgl_create_context_attribs_arb;
    PFNWGLCHOOSEPIXELFORMATARBPROC wgl_choose_pixel_format_arb;

    if (RegisterClassExW(&window_class) == 0) {
        return false;
    }

    window = CreateWindowExW(
        0,
        window_class_name,
        L"Shader Testbed",
        0,
        0,
        0,
        0,
        0,
        NULL,
        NULL,
        instance,
        NULL
    );
    if (window == NULL) {
        goto out_unregister_window_class;
    }

    device_context = GetDC(window);
    if (device_context == NULL) {
        goto out_destroy_window;
    }

    suggested_pixel_format_index = ChoosePixelFormat(device_context, &pixel_format);
    if (suggested_pixel_format_index == 0) {
        goto out_release_device_context;
    }

    if (DescribePixelFormat(
        device_context,
        suggested_pixel_format_index,
        sizeof(PIXELFORMATDESCRIPTOR),
        &pixel_format
    ) == 0) {
        goto out_release_device_context;
    }

    if (SetPixelFormat(device_context, suggested_pixel_format_index, &pixel_format) == FALSE) {
        goto out_release_device_context;
    }

    rendering_context = wglCreateContext(device_context);
    if (rendering_context == NULL) {
        goto out_release_device_context;
    }

    if (wglMakeCurrent(device_context, rendering_context) == FALSE) {
        goto out_delete_rendering_context;
    }

    // Using wglGetProcAddress requires wily casting.
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"

    wgl_choose_pixel_format_arb = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
    if (wgl_choose_pixel_format_arb == NULL) {
        goto out_reset_current_rendering_context;
    }

    wgl_create_context_attribs_arb = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    if (wgl_create_context_attribs_arb == NULL) {
        goto out_reset_current_rendering_context;
    }

    #pragma GCC diagnostic pop

    wglChoosePixelFormatARB = wgl_choose_pixel_format_arb;
    wglCreateContextAttribsARB = wgl_create_context_attribs_arb;
    success = true;

out_reset_current_rendering_context:
    wglMakeCurrent(NULL, NULL);
out_delete_rendering_context:
    wglDeleteContext(rendering_context);
out_release_device_context:
    ReleaseDC(window, device_context);
out_destroy_window:
    DestroyWindow(window);
out_unregister_window_class:
    UnregisterClassW(window_class_name, instance);
    return success;
}

// Separate from context creation extensions,
// because wglGetProcAddress can return different addresses for different contexts.
static bool init_gl_extensions(void)
{
    // Using wglGetProcAddress requires wily casting.
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"

    #define INIT_PROC(type, proc) \
        do { \
            proc = (type)wglGetProcAddress(#proc); \
            if (proc == NULL) { \
                return false; \
            } \
        } while (false)

    INIT_PROC(PFNGLBINDBUFFERPROC, glBindBuffer);
    INIT_PROC(PFNGLGENBUFFERSPROC, glGenBuffers);
    INIT_PROC(PFNGLBUFFERDATAPROC, glBufferData);
    INIT_PROC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
    INIT_PROC(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
    INIT_PROC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
    INIT_PROC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
    INIT_PROC(PFNGLCREATESHADERPROC, glCreateShader);
    INIT_PROC(PFNGLDELETESHADERPROC, glDeleteShader);
    INIT_PROC(PFNGLSHADERSOURCEPROC, glShaderSource);
    INIT_PROC(PFNGLCOMPILESHADERPROC, glCompileShader);
    INIT_PROC(PFNGLGETSHADERIVPROC, glGetShaderiv);
    INIT_PROC(PFNGLCREATEPROGRAMPROC, glCreateProgram);
    INIT_PROC(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
    INIT_PROC(PFNGLATTACHSHADERPROC, glAttachShader);
    INIT_PROC(PFNGLLINKPROGRAMPROC, glLinkProgram);
    INIT_PROC(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
    INIT_PROC(PFNGLUSEPROGRAMPROC, glUseProgram);
    INIT_PROC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
    INIT_PROC(PFNGLUNIFORM1FPROC, glUniform1f);
    INIT_PROC(PFNGLUNIFORM1IPROC, glUniform1i);
    INIT_PROC(PFNGLACTIVETEXTUREPROC, glActiveTexture);

    #undef INIT_PROC

    #pragma GCC diagnostic pop

    return true;
}

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

    if (!init_gl_context_creation_extensions(instance)) {
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

    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_ALPHA_BITS_ARB, 8,
        0
    };
    int suggested_pixel_format_index;
    UINT pixel_format_count;
    if (wglChoosePixelFormatARB(
        device_context,
        pixel_format_attribs,
        NULL,
        1,
        &suggested_pixel_format_index,
        &pixel_format_count
    ) == FALSE) {
        show_error(L"wglChoosePixelFormatARB() failed.");
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

    int context_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    HGLRC rendering_context = wglCreateContextAttribsARB(device_context, NULL, context_attribs);
    if (rendering_context == NULL) {
        show_error(L"wglCreateContext() failed.");
        return -1;
    }

    if (wglMakeCurrent(device_context, rendering_context) == FALSE) {
        show_error(L"wglMakeCurrent() failed.");
        return -1;
    }

    if (!init_gl_extensions()) {
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
        testbed_update((GLsizei)s_width, (GLsizei)s_height, time, delta_time);
        previous_performance_counter = performance_counter;

        SwapBuffers(device_context);
    }

    return 0;
}
