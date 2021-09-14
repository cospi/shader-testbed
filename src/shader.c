#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

#include "gl.h"

// Using fseek and ftell can be unreliable, but seems to work on the supported platforms.
static char *read_text_file(const char *path)
{
    char *text = NULL;
    FILE *file;
    long size;

    if (path == NULL) {
        return NULL;
    }

    file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);

    size = ftell(file);
    if (size <= 0L) {
        goto out_close_file;
    }

    text = malloc((size_t)(size + 1 /* Null character */));
    if (text == NULL) {
        goto out_close_file;
    }

    rewind(file);
    if (fread(text, (size_t)size, 1, file) != 1) {
        free(text);
        text = NULL;
        goto out_close_file;
    }
    text[size] = '\0';

out_close_file:
    fclose(file);
    return text;
}

static GLuint create_shader_from_source(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static GLuint create_shader_from_path(GLenum type, const char *path)
{
    char *source = read_text_file(path);
    if (source == NULL) {
        return 0;
    }

    GLuint shader = create_shader_from_source(type, source);
    free(source);
    return shader;
}

static GLuint create_program_from_shaders(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();
    if (program == 0) {
        return 0;
    }

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

static GLuint create_program_from_paths(const char *vertex_shader_path, const char *fragment_shader_path)
{
    GLuint vertex_shader = create_shader_from_path(GL_VERTEX_SHADER, vertex_shader_path);
    if (vertex_shader == 0) {
        return 0;
    }

    GLuint fragment_shader = create_shader_from_path(GL_FRAGMENT_SHADER, fragment_shader_path);
    if (fragment_shader == 0) {
        glDeleteShader(vertex_shader);
        return 0;
    }

    GLuint program = create_program_from_shaders(vertex_shader, fragment_shader);
    // Vertex and fragment shader are no more necessary after the program has been created.
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

Shader *shader_create(const char *vertex_shader_path, const char *fragment_shader_path)
{
    GLuint program = create_program_from_paths(vertex_shader_path, fragment_shader_path);
    if (program == 0) {
        return NULL;
    }

    Shader *shader = malloc(sizeof(Shader));
    if (shader == NULL) {
        glDeleteProgram(program);
        return NULL;
    }

    shader->program = program;
    return shader;
}

void shader_destroy(Shader *shader)
{
    if (shader != NULL) {
        glDeleteProgram(shader->program);
        free(shader);
    }
}
