#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

#include "gl.h"

// Using fseek and ftell can be unrealiable, but seems to work on the supported platforms.
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
    fread(text, 1, (size_t)size, file);
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

static GLuint create_program(GLuint vertex_shader, GLuint fragment_shader)
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

Shader *shader_create(const char *vertex_shader_path, const char *fragment_shader_path)
{
    GLuint vertex_shader, fragment_shader, program;
    Shader *shader;

    vertex_shader = create_shader_from_path(GL_VERTEX_SHADER, vertex_shader_path);
    if (vertex_shader == 0) {
        return NULL;
    }

    fragment_shader = create_shader_from_path(GL_FRAGMENT_SHADER, fragment_shader_path);
    if (fragment_shader == 0) {
        goto error_delete_vertex_shader;
    }

    program = create_program(vertex_shader, fragment_shader);
    if (program == 0) {
        goto error_delete_fragment_shader;
    }

    shader = malloc(sizeof(Shader));
    if (shader == NULL) {
        goto error_delete_program;
    }

    shader->program = program;
    // Vertex and fragment shader are no more necessary after the program has been created.
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader;

error_delete_program:
    glDeleteProgram(program);
error_delete_fragment_shader:
    glDeleteShader(fragment_shader);
error_delete_vertex_shader:
    glDeleteShader(vertex_shader);
    return NULL;
}
