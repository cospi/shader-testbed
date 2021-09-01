#ifndef SHADER_TESTBED_MESH_H_
#define SHADER_TESTBED_MESH_H_

#include <stddef.h>

#include "gl.h"
#include "vertex_attribute_descriptor.h"

typedef struct Mesh {
    GLuint vertex_array;
    GLuint buffers[2];
    size_t index_count;
} Mesh;

void mesh_init(
    Mesh *mesh,
    const VertexAttributeDescriptor *vertex_attribute_descriptors,
    size_t vertex_attribute_descriptor_count,
    GLsizei vertex_attribute_layout_stride,
    const void *vertices,
    GLsizeiptr vertices_size,
    const GLushort *indices,
    size_t index_count
);
void mesh_draw(Mesh *mesh);

#endif // SHADER_TESTBED_MESH_H_
