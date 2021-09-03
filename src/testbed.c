#include "testbed.h"

#include "geometry.h"
#include "gl.h"
#include "math.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "vertex.h"

typedef struct ShaderInfo {
    Shader *shader;
    GLint time_uniform;
    GLint delta_time_uniform;
    GLint transform_uniform;
} ShaderInfo;

#define TEXTURE_COUNT 4

static const char *TEXTURE_TGA_PATHS[TEXTURE_COUNT] = {
    "res/images/texture_0.tga",
    "res/images/texture_1.tga",
    "res/images/texture_2.tga",
    "res/images/texture_3.tga"
};
static const char *TEXTURE_UNIFORM_NAMES[TEXTURE_COUNT] = {
    "u_texture_0",
    "u_texture_1",
    "u_texture_2",
    "u_texture_3"
};

#define FOV (60.0f * MATH_DEG2RAD)
#define NEAR_PLANE 0.0f
#define FAR_PLANE -1000.0f

static const Vector3 CUBE_POSITION = { .x = 0.0f, .y = 0.0f, .z = -2.0f };
static const Vector3 CUBE_ROTATION_AXIS = { .x = 0.70710678118f, .y = 0.70710678118f, .z = 0.0f };

static Mesh s_quad_mesh = { 0 };
static ShaderInfo s_quad_shader_info = { 0 };
static Mesh s_cube_mesh = { 0 };
static ShaderInfo s_cube_shader_info = { 0 };
static Texture *s_textures[TEXTURE_COUNT] = { NULL, NULL, NULL, NULL };

static void shader_info_init(ShaderInfo *shader_info, const char *vertex_shader_path, const char *fragment_shader_path)
{
    Shader *shader = shader_create(vertex_shader_path, fragment_shader_path);
    if (shader != NULL) {
        shader_info->shader = shader;
        GLuint program = shader->program;
        shader_info->time_uniform = glGetUniformLocation(program, "u_time");
        shader_info->delta_time_uniform = glGetUniformLocation(program, "u_delta_time");
        shader_info->transform_uniform = glGetUniformLocation(program, "u_transform");
        glUseProgram(program);
        for (int i = 0; i < TEXTURE_COUNT; ++i) {
            GLint texture_uniform = glGetUniformLocation(program, TEXTURE_UNIFORM_NAMES[i]);
            if (texture_uniform != -1) {
                glUniform1i(texture_uniform, i);
            }
        }
    }
}

static void shader_info_fini(ShaderInfo *shader_info)
{
    Shader *shader = shader_info->shader;
    if (shader != NULL) {
        shader_destroy(shader);
        shader_info->shader = NULL;
        shader_info->time_uniform = -1;
        shader_info->delta_time_uniform = -1;
    }
}

static void init_quad_mesh(void)
{
    Vertex vertices[GEOMETRY_QUAD_VERTEX_COUNT];
    GLushort indices[GEOMETRY_QUAD_INDEX_COUNT];
    geometry_init_quad(vertices, indices, 1.0f);
    mesh_init(&s_quad_mesh, vertices, GEOMETRY_QUAD_VERTEX_COUNT, indices, GEOMETRY_QUAD_INDEX_COUNT);
}

static void init_quad_shader(void)
{
    shader_info_init(&s_quad_shader_info, "res/shaders/quad.vert", "res/shaders/quad.frag");
}

static void reload_quad_shader(void)
{
    shader_info_fini(&s_quad_shader_info);
    init_quad_shader();
}

static void init_cube_mesh(void)
{
    Vertex vertices[GEOMETRY_CUBE_VERTEX_COUNT];
    GLushort indices[GEOMETRY_CUBE_INDEX_COUNT];
    geometry_init_cube(vertices, indices, 0.5f);
    mesh_init(&s_cube_mesh, vertices, GEOMETRY_CUBE_VERTEX_COUNT, indices, GEOMETRY_CUBE_INDEX_COUNT);
}

static void init_cube_shader(void)
{
    shader_info_init(&s_cube_shader_info, "res/shaders/cube.vert", "res/shaders/cube.frag");
}

static void reload_cube_shader(void)
{
    shader_info_fini(&s_cube_shader_info);
    init_cube_shader();
}

static void init_textures(void)
{
    for (int i = 0; i < TEXTURE_COUNT; ++i) {
        glActiveTexture((GLenum)(GL_TEXTURE0 + i));
        s_textures[i] = texture_create_from_tga(TEXTURE_TGA_PATHS[i]);
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

static void draw_mesh(
    const Mesh *mesh,
    const ShaderInfo *shader_info,
    float time,
    float delta_time,
    const Matrix4x4 transform
)
{
    const Shader *shader = shader_info->shader;
    if (shader != NULL) {
        GLuint program = shader->program;
        glUseProgram(program);
        glUniform1f(shader_info->time_uniform, time);
        glUniform1f(shader_info->delta_time_uniform, delta_time);
        glUniformMatrix4fv(shader_info->transform_uniform, 1, GL_TRUE, transform);
        mesh_draw(mesh);
    }
}

static void draw_quad_mesh(float time, float delta_time)
{
    Matrix4x4 transform;
    matrix4x4_identity(transform);
    draw_mesh(&s_quad_mesh, &s_quad_shader_info, time, delta_time, transform);
}

static void draw_cube_mesh(GLsizei width, GLsizei height, float time, float delta_time)
{
    Matrix4x4 translate, rotate, model, projection, transform;
    matrix4x4_translate(translate, CUBE_POSITION);
    matrix4x4_rotate(rotate, CUBE_ROTATION_AXIS, time);
    matrix4x4_multiply(model, translate, rotate);
    matrix4x4_perspective(projection, FOV, (float)width / (float)height, NEAR_PLANE, FAR_PLANE);
    matrix4x4_multiply(transform, projection, model);
    draw_mesh(&s_cube_mesh, &s_cube_shader_info, time, delta_time, transform);
}

void testbed_init(void)
{
    init_quad_mesh();
    init_quad_shader();
    init_cube_mesh();
    init_cube_shader();
    init_textures();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void testbed_update(GLsizei width, GLsizei height, float time, float delta_time)
{
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_quad_mesh(time, delta_time);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw_cube_mesh(width, height, time, delta_time);
}

void testbed_reload(void)
{
    reload_quad_shader();
    reload_cube_shader();
    reload_textures();
}
