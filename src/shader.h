#ifndef SHADER_TESTBED_SHADER_H_
#define SHADER_TESTBED_SHADER_H_

#include <stddef.h>

#include <GL/gl.h>

typedef struct Shader {
    GLuint program;
} Shader;

Shader *shader_create(const char *vertex_shader_path, const char *fragment_shader_path);
void shader_destroy(Shader *shader);

#endif // SHADER_TESTBED_SHADER_H_
