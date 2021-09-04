#include <stdio.h>

#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "glx.h"
#include "../testbed.h"

static void print_error(const char *error)
{
    fprintf(stderr, "%s\n", error);
}

int main(void)
{
    // Resource cleanup is left to the OS.

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
    bool running = true;
    int width = 1280, height = 720;
    float time = 0.0f;
    Display *display;
    int glx_major, glx_minor, screen, fb_config_count;
    GLXFBConfig *fb_configs, fb_config;
    XVisualInfo *visual_info;
    Visual *visual;
    Window root_window;
    Colormap colormap;
    XSetWindowAttributes window_attribs;
    Window window;
    Atom wm_delete_window;
    GLXContext context;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        print_error("XOpenDisplay() failed.");
        return -1;
    }

    screen = DefaultScreen(display);
    if (!glx_init_context_creation_extensions(display, screen)) {
        print_error("Initializing OpenGL context creation extensions failed.");
        return -1;
    }

    // FBConfigs require GLX 1.3 or later.
    if (
        (glXQueryVersion(display, &glx_major, &glx_minor) == False)
        || (glx_major < 1)
        || ((glx_major == 1) && (glx_minor < 3))
    ) {
        print_error("Unsupported GLX version.");
        return -1;
    }

    fb_configs = glXChooseFBConfig(display, screen, fb_config_attribs, &fb_config_count);
    if (fb_configs == NULL) {
        print_error("No FB configs found.");
        return -1;
    }

    fb_config = fb_configs[0];
    XFree(fb_configs);

    visual_info = glXGetVisualFromFBConfig(display, fb_config);
    visual = visual_info->visual;
    root_window = RootWindow(display, screen);
    colormap = XCreateColormap(display, root_window, visual, AllocNone);

    window_attribs.colormap = colormap;
    window_attribs.background_pixmap = None;
    window_attribs.border_pixel = 0;
    window_attribs.event_mask = StructureNotifyMask | KeyPressMask;
    window = XCreateWindow(
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

    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    context = glx_create_context(display, fb_config);
    if (context == NULL) {
        print_error("Creating OpenGL context failed.");
        return -1;
    }

    glXMakeCurrent(display, window, context);

    if (!glx_init_extensions()) {
        print_error("Initializing OpenGL extensions failed.");
        return -1;
    }

    testbed_init();

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

        // TODO: Proper timing
        time += 0.01f;
        testbed_update((GLsizei)width, (GLsizei)height, time, 0.01f);

        glXSwapBuffers(display, window);
    }

    return 0;
}
