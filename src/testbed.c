#include "testbed.h"

#include "gl.h"
#include "shader.h"

static bool s_initialized = false;
static Shader *s_shader = NULL;
static GLint s_time_uniform_location = -1;
static GLint s_delta_time_uniform_location = -1;
static GLuint s_vertex_array = 0;
static GLuint s_buffers[2] = { 0, 0 };

static bool init_shader(void)
{
    if (s_shader != NULL) {
        return true;
    }

    s_shader = shader_create("res/shaders/vertex.shader", "res/shaders/fragment.shader");
    if (s_shader == NULL) {
        return false;
    }

    GLuint program = s_shader->program;
    s_time_uniform_location = glGetUniformLocation(program, "u_time");
    s_delta_time_uniform_location = glGetUniformLocation(program, "u_delta_time");
    // Time uniforms not existing is not considered an error. Shader will just not have access to time.
    return true;
}

bool testbed_init(void)
{
    if (s_initialized) {
        return true;
    }

    if (!init_shader()) {
        return false;
    }

    glGenVertexArrays(1, &s_vertex_array);
    glBindVertexArray(s_vertex_array);

    glGenBuffers(2, s_buffers);

    glBindBuffer(GL_ARRAY_BUFFER, s_buffers[0]);
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_buffers[1]);
    GLushort indices[] = { 0, 1, 2 };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));

    s_initialized = true;
    return true;
}

void testbed_update(GLsizei width, GLsizei height, long double time, long double delta_time)
{
    if (!s_initialized) {
        return;
    }

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(s_shader->program);
    if (s_time_uniform_location != -1) {
        glUniform1f(s_time_uniform_location, (float)time);
    }
    if (s_delta_time_uniform_location != -1) {
        glUniform1f(s_delta_time_uniform_location, (float)delta_time);
    }
    glBindVertexArray(s_vertex_array);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);
}
