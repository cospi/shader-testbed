#ifndef SHADER_TESTBED_SHADER_H_
#define SHADER_TESTBED_SHADER_H_

#include <gl/GL.h>

typedef struct Shader {
    GLuint program;
} Shader;

Shader *shader_create(const char *vertex_shader_path, const char *fragment_shader_path);

#endif // SHADER_TESTBED_SHADER_H_
