#ifndef SHADER_TESTBED_X11_GLX_H_
#define SHADER_TESTBED_X11_GLX_H_

#include <stdbool.h>

#include <GL/glx.h>
#include <X11/Xlib.h>

bool glx_init_context_creation_extensions(Display *display, int screen);
GLXContext glx_create_context(Display *display, GLXFBConfig fb_config);
bool glx_init_extensions(void);

#endif // SHADER_TESTBED_X11_GLX_H_
