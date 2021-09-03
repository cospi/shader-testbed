#ifndef SHADER_TESTBED_WIN32_WGL_H_
#define SHADER_TESTBED_WIN32_WGL_H_

#include <stdbool.h>

#include <Windows.h>

bool wgl_init_context_creation_extensions(HINSTANCE instance);
HGLRC wgl_create_context(HDC device_context);
bool wgl_init_extensions(void);

#endif // SHADER_TESTBED_WIN32_WGL_H_
