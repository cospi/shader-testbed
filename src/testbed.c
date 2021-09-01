#include "testbed.h"

#include "gl.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "vertex_attribute_descriptor.h"

#define TEXTURE_COUNT 4

static const char *TEXTURE_TGA_PATHS[TEXTURE_COUNT] =
    { "res/images/texture_0.tga", "res/images/texture_1.tga", "res/images/texture_2.tga", "res/images/texture_3.tga" };
static const char *TEXTURE_UNIFORM_NAMES[TEXTURE_COUNT] =
    { "u_texture_0", "u_texture_1", "u_texture_2", "u_texture_3" };

static Shader *s_shader = NULL;
static GLint s_time_uniform_location = -1;
static GLint s_delta_time_uniform_location = -1;
static Texture *s_textures[TEXTURE_COUNT] = { NULL, NULL, NULL, NULL };
static Mesh s_quad_mesh = { 0 };

static void init_shader(void)
{
    s_shader = shader_create("res/shaders/vertex.shader", "res/shaders/fragment.shader");
    if (s_shader != NULL) {
        GLuint program = s_shader->program;
        // Time uniforms not existing is not considered an error. Shader will just not have access to time.
        s_time_uniform_location = glGetUniformLocation(program, "u_time");
        s_delta_time_uniform_location = glGetUniformLocation(program, "u_delta_time");
    }
}

static void reload_shader(void)
{
    if (s_shader != NULL) {
        shader_destroy(s_shader);
        s_shader = NULL;
        s_time_uniform_location = -1;
        s_delta_time_uniform_location = -1;
    }
    init_shader();
}

static void init_textures(void)
{
    if (s_shader == NULL) {
        return;
    }

    GLuint program = s_shader->program;
    glUseProgram(program);
    // Textures missing is not considered an error. Shader will just not have access to missing textures.
    for (int i = 0; i < TEXTURE_COUNT; ++i) {
        s_textures[i] = texture_create_from_tga(TEXTURE_TGA_PATHS[i]);
        GLint uniform_location = glGetUniformLocation(program, TEXTURE_UNIFORM_NAMES[i]);
        if (uniform_location != -1) {
            glUniform1i(uniform_location, i);
        }
    }
}

static void reload_textures(void)
{
    for (int i = 0; i < TEXTURE_COUNT; ++i) {
        Texture *texture = s_textures[i];
        if (texture != NULL) {
            texture_destroy(texture);
            s_textures[i] = NULL;
        }
    }
    init_textures();
}

static void init_quad_mesh(void)
{
    VertexAttributeDescriptor vertex_attribute_descriptors[] = {
        { .index = 0, .dimension = 3, .type = GL_FLOAT, .normalized = GL_FALSE, .offset = 0 },
        { .index = 1, .dimension = 2, .type = GL_FLOAT, .normalized = GL_FALSE, .offset = 0 }
    };
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    mesh_init(
        &s_quad_mesh,
        vertex_attribute_descriptors,
        2,
        5 * sizeof(GLfloat),
        vertices,
        sizeof(vertices),
        indices,
        6
    );
}

void testbed_init(void)
{
    init_shader();
    init_textures();
    init_quad_mesh();
}

void testbed_update(GLsizei width, GLsizei height, long double time, long double delta_time)
{
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (s_shader != NULL) {
        glUseProgram(s_shader->program);
        if (s_time_uniform_location != -1) {
            glUniform1f(s_time_uniform_location, (float)time);
        }
        if (s_delta_time_uniform_location != -1) {
            glUniform1f(s_delta_time_uniform_location, (float)delta_time);
        }
    }

    for (int i = 0; i < TEXTURE_COUNT; ++i) {
        const Texture *texture = s_textures[i];
        if (texture != NULL) {
            glActiveTexture((GLenum)(GL_TEXTURE0 + i));
            glBindTexture(GL_TEXTURE_2D, texture->texture);
        }
    }

    mesh_draw(&s_quad_mesh);
}

void testbed_reload(void)
{
    reload_shader();
    reload_textures();
}
