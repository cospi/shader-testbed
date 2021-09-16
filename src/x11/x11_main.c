#include <stdio.h>
#include <time.h>

#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "glx.h"
#include "../testbed.h"

#define NSEC_PER_SEC 1000000000LL

static void print_error(const char *error)
{
    fprintf(stderr, "%s\n", error);
}

static long long get_time_nsec(void)
{
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &time) == -1) {
        return 0LL;
    }
    return ((long long)time.tv_sec * NSEC_PER_SEC) + time.tv_nsec;
}

static float nsec_to_sec(long long nsec)
{
    return (float)((long double)nsec / NSEC_PER_SEC);
}

int main(void)
{
    // Resource cleanup is left to the OS.

    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        print_error("XOpenDisplay() failed.");
        return -1;
    }

    int screen = DefaultScreen(display);
    if (!glx_init_context_creation_extensions(display, screen)) {
        print_error("Initializing OpenGL context creation extensions failed.");
        return -1;
    }

    // FBConfigs require GLX 1.3 or later.
    int glx_major, glx_minor;
    if (
        (glXQueryVersion(display, &glx_major, &glx_minor) == False)
        || (glx_major < 1)
        || ((glx_major == 1) && (glx_minor < 3))
    ) {
        print_error("Unsupported GLX version.");
        return -1;
    }

    int fb_config_attribs[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_DOUBLEBUFFER, True,
        None
    };
    int fb_config_count;
    GLXFBConfig *fb_configs = glXChooseFBConfig(display, screen, fb_config_attribs, &fb_config_count);
    if (fb_configs == NULL) {
        print_error("No FB configs found.");
        return -1;
    }

    GLXFBConfig fb_config = fb_configs[0];
    XFree(fb_configs);

    XVisualInfo *visual_info = glXGetVisualFromFBConfig(display, fb_config);
    Visual *visual = visual_info->visual;
    Window root_window = RootWindow(display, screen);
    Colormap colormap = XCreateColormap(display, root_window, visual, AllocNone);

    int width = 1280;
    int height = 720;
    XSetWindowAttributes window_attribs;
    window_attribs.colormap = colormap;
    window_attribs.background_pixmap = None;
    window_attribs.border_pixel = 0;
    window_attribs.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
    Window window = XCreateWindow(
        display,
        root_window,
        0,
        0,
        (unsigned int)width,
        (unsigned int)height,
        0,
        visual_info->depth,
        InputOutput,
        visual,
        CWBorderPixel | CWColormap | CWEventMask,
        &window_attribs
    );

    XFree(visual_info);

    XStoreName(display, window, "Shader Testbed");
    XMapWindow(display, window);

    for (;;) {
        XEvent event;
        XNextEvent(display, &event);
        if ((event.type == Expose) && (event.xexpose.count == 0)) {
            break;
        }
    }

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    GLXContext context = glx_create_context(display, fb_config);
    if (context == NULL) {
        print_error("Creating OpenGL context failed.");
        return -1;
    }

    glXMakeCurrent(display, window, context);

    if (!glx_init_extensions()) {
        print_error("Initializing OpenGL extensions failed.");
        return -1;
    }

    if (!testbed_init()) {
        print_error("Initializing testbed failed.");
    }

    long long time_nsec = 0LL;
    long long previous_time_nsec = get_time_nsec();

    bool running = true;
    while (running) {
        while (XPending(display) > 0) {
            XEvent event;
            XNextEvent(display, &event);
            switch (event.type) {
            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == wm_delete_window) {
                    running = false;
                }
                break;
            case ConfigureNotify:
                width = event.xconfigure.width;
                height = event.xconfigure.height;
                break;
            case KeyPress:
                if (XLookupKeysym(&event.xkey, 0) == XK_F5) {
                    testbed_reload();
                }
                break;
            default:
                break;
            }
        }

        long long current_time_nsec = get_time_nsec();
        long long delta_time_nsec = (current_time_nsec - previous_time_nsec);
        time_nsec += delta_time_nsec;
        testbed_update((GLsizei)width, (GLsizei)height, nsec_to_sec(time_nsec), nsec_to_sec(delta_time_nsec));
        previous_time_nsec = current_time_nsec;

        glXSwapBuffers(display, window);
    }

    return 0;
}
