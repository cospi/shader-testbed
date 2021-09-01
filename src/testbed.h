#ifndef SHADER_TESTBED_TESTBED_H_
#define  SHADER_TESTBED_TESTBED_H_

#include <stdbool.h>

#include <gl/GL.h>

void testbed_init(void);
void testbed_update(GLsizei width, GLsizei height, long double time, long double delta_time);
void testbed_reload(void);

#endif // SHADER_TESTBED_TESTBED_H_
