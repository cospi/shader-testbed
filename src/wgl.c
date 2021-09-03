#include "wgl.h"

#include "gl.h"

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

// SetPixelFormat can only be called once on a device context.
// Therefore, a dummy device context is necessary for initializing the context creation extensions.
bool wgl_init_context_creation_extensions(HINSTANCE instance)
{
    wchar_t window_class_name[] = L"ShaderTestbedWGLExtensionsWindowClass";
    WNDCLASSEXW window_class = {
        .cbSize = sizeof(WNDCLASSEXW),
        .style = CS_OWNDC,
        .lpfnWndProc = DefWindowProcW,
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
        .cAlphaBits = 8,
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

HGLRC wgl_create_context(HDC device_context)
{
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
        return NULL;
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
        .cAlphaBits = 8,
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
        return NULL;
    }

    if (SetPixelFormat(device_context, suggested_pixel_format_index, &pixel_format) == FALSE) {
        return NULL;
    }

    int context_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    return wglCreateContextAttribsARB(device_context, NULL, context_attribs);
}

// Separate from context creation extensions,
// because wglGetProcAddress can return different addresses for different contexts.
bool wgl_init_extensions(void)
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
    INIT_PROC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
    INIT_PROC(PFNGLACTIVETEXTUREPROC, glActiveTexture);

    #undef INIT_PROC

    #pragma GCC diagnostic pop

    return true;
}
