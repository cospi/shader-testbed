#ifndef SHADER_TESTBED_MESH_H_
#define SHADER_TESTBED_MESH_H_

#include <stddef.h>

#include "gl.h"
#include "vertex.h"

typedef struct Mesh {
    GLuint vertex_array;
    GLuint buffers[2];
    size_t index_count;
} Mesh;

void mesh_init(Mesh *mesh, const Vertex *vertices, size_t vertex_count, const GLushort *indices, size_t index_count);
void mesh_draw(const Mesh *mesh);

#endif // SHADER_TESTBED_MESH_H_
