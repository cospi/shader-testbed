#include "mesh.h"

#include "gl.h"

void mesh_init(
    Mesh *mesh,
    const VertexAttributeDescriptor *vertex_attribute_descriptors,
    size_t vertex_attribute_descriptor_count,
    GLsizei vertex_attribute_layout_stride,
    const void *vertices,
    GLsizeiptr vertices_size,
    const GLushort *indices,
    size_t index_count
)
{
    if (mesh == NULL) {
        return;
    }

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    mesh->vertex_array = vertex_array;

    GLuint *buffers = mesh->buffers;
    glGenBuffers(2, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(index_count * sizeof(GLushort)), indices, GL_STATIC_DRAW);

    for (size_t i = 0; i < vertex_attribute_descriptor_count; ++i) {
        const VertexAttributeDescriptor *vertex_attribute_descriptor = vertex_attribute_descriptors + i;
        GLuint index = vertex_attribute_descriptor->index;
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(
            index,
            vertex_attribute_descriptor->dimension,
            vertex_attribute_descriptor->type,
            vertex_attribute_descriptor->normalized,
            vertex_attribute_layout_stride,
            (GLvoid *)vertex_attribute_descriptor->offset
        );
    }

    mesh->index_count = index_count;
}

void mesh_draw(Mesh *mesh)
{
    if (mesh != NULL) {
        glBindVertexArray(mesh->vertex_array);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh->index_count, GL_UNSIGNED_SHORT, NULL);
    }
}
