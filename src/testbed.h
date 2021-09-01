#ifndef SHADER_TESTBED_TESTBED_H_
#define  SHADER_TESTBED_TESTBED_H_

#include <stdbool.h>

#include <gl/GL.h>

bool testbed_init(void);
void testbed_update(GLsizei width, GLsizei height, long double time, long double delta_time);

#endif // SHADER_TESTBED_TESTBED_H_
