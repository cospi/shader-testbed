#include "glx.h"

#include <string.h>

#include <GL/glx.h>

#include "../gl.h"

typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
static PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = NULL;

typedef int (*XErrorHandler)(Display *, XErrorEvent *);

static bool s_error = false;

static bool has_extension(const char *extensions, const char *extension)
{
    // Extension names shouldn't contain spaces.
    if (strchr(extension, ' ') != NULL) {
        return false;
    }

    size_t extension_length = strlen(extension);
    if (extension_length == 0) {
        return false;
    }

    for (const char *start = extensions;;) {
        const char *found = strstr(start, extension);
        if (found == NULL) {
            break;
        }

        const char *remaining = found + extension_length;
        if ((found == start) || (found[-1] == ' ')) {
            char terminator = *remaining;
            if ((terminator == '\0') || (terminator == ' ')) {
                return true;
            }
        }

        start = remaining;
    }
    return false;
}

static int signal_error(Display *display, XErrorEvent *error_event)
{
    (void)display;
    (void)error_event;

    s_error = true;
    return 0;
}

bool glx_init_context_creation_extensions(Display *display, int screen)
{
    const char *extensions = glXQueryExtensionsString(display, screen);
    if (!has_extension(extensions, "GLX_ARB_create_context")) {
        return false;
    }

    PFNGLXCREATECONTEXTATTRIBSARBPROC glx_create_context_attribs_arb =
        (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");
    if (glx_create_context_attribs_arb == NULL) {
        return false;
    }

    glXCreateContextAttribsARB = glx_create_context_attribs_arb;
    return true;
}

GLXContext glx_create_context(Display *display, GLXFBConfig fb_config)
{
    // Prevent abort (default X11 error resolution) in case context creation fails.
    s_error = false;
    XErrorHandler previous_error_handler = XSetErrorHandler(&signal_error);
    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 5,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };
    GLXContext context = glXCreateContextAttribsARB(display, fb_config, NULL, True, context_attribs);

    XSync(display, False);
    XSetErrorHandler(previous_error_handler);
    if (s_error) {
        s_error = false;
        if (context != NULL) {
            glXDestroyContext(display, context);
        }
        return NULL;
    }

    return context;
}

bool glx_init_extensions(void)
{
    #define INIT_PROC(type, proc) \
        do { \
            proc = (type)glXGetProcAddressARB((const GLubyte *)#proc); \
            if (proc == NULL) { \
                return false; \
            } \
        } while (false)

    INIT_PROC(PFNGLBINDBUFFERPROC, glBindBuffer);
    INIT_PROC(PFNGLGENBUFFERSPROC, glGenBuffers);
    INIT_PROC(PFNGLBUFFERDATAPROC, glBufferData);
    INIT_PROC(PFNGLMAPBUFFERPROC, glMapBuffer);
    INIT_PROC(PFNGLUNMAPBUFFERPROC, glUnmapBuffer);
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

    #undef INIT_PROC

    return true;
}
