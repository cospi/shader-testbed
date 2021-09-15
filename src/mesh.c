#include "mesh.h"

#include "gl.h"

bool mesh_init(Mesh *mesh, const Vertex *vertices, size_t vertex_count, const GLushort *indices, size_t index_count)
{
    if (mesh == NULL) {
        return false;
    }

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    mesh->vertex_array = vertex_array;
    mesh->index_count = index_count;

    GLuint *buffers = mesh->buffers;
    glGenBuffers(2, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vertex_count * sizeof(Vertex)), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(index_count * sizeof(GLushort)), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, uv));

    return true;
}

void mesh_draw(const Mesh *mesh)
{
    if (mesh != NULL) {
        glBindVertexArray(mesh->vertex_array);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh->index_count, GL_UNSIGNED_SHORT, NULL);
    }
}
