#ifndef SHADER_TESTBED_TESTBED_H_
#define  SHADER_TESTBED_TESTBED_H_

#include <stdbool.h>

#include <GL/gl.h>

bool testbed_init(void);
void testbed_update(GLsizei width, GLsizei height, float time, float delta_time);
void testbed_reload(void);

#endif // SHADER_TESTBED_TESTBED_H_
