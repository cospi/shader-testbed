#include "geometry.h"

#include <string.h>

static Vertex *push_vertex(
    Vertex *vertices,
    float position_x,
    float position_y,
    float position_z,
    float normal_x,
    float normal_y,
    float normal_z,
    float u,
    float v
)
{
    vertices->position.x = position_x;
    vertices->position.y = position_y;
    vertices->position.z = position_z;
    vertices->normal.x = normal_x;
    vertices->normal.y = normal_y;
    vertices->normal.z = normal_z;
    vertices->uv.x = u;
    vertices->uv.y = v;
    return vertices + 1;
}

static Vertex *push_quad_vertices(Vertex *vertices, float extents)
{
    vertices = push_vertex(vertices, -extents, -extents, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    vertices = push_vertex(vertices, +extents, -extents, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
    vertices = push_vertex(vertices, +extents, +extents, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    vertices = push_vertex(vertices, -extents, +extents, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    return vertices;
}

static GLushort *push_quad_indices(GLushort *indices, GLushort offset)
{
    *(indices++) = offset;
    *(indices++) = (GLushort)(offset + 1);
    *(indices++) = (GLushort)(offset + 2);
    *(indices++) = offset;
    *(indices++) = (GLushort)(offset + 2);
    *(indices++) = (GLushort)(offset + 3);
    return indices;
}

static Vertex *push_cube_vertices(Vertex *vertices, float extents)
{
    // Front
    vertices = push_vertex(vertices, -extents, -extents, +extents, +0.0f, +0.0f, +1.0f, 0.0f, 0.0f);
    vertices = push_vertex(vertices, +extents, -extents, +extents, +0.0f, +0.0f, +1.0f, 1.0f, 0.0f);
    vertices = push_vertex(vertices, +extents, +extents, +extents, +0.0f, +0.0f, +1.0f, 1.0f, 1.0f);
    vertices = push_vertex(vertices, -extents, +extents, +extents, +0.0f, +0.0f, +1.0f, 0.0f, 1.0f);
    // Back
    vertices = push_vertex(vertices, +extents, -extents, -extents, +0.0f, +0.0f, -1.0f, 0.0f, 0.0f);
    vertices = push_vertex(vertices, -extents, -extents, -extents, +0.0f, +0.0f, -1.0f, 1.0f, 0.0f);
    vertices = push_vertex(vertices, -extents, +extents, -extents, +0.0f, +0.0f, -1.0f, 1.0f, 1.0f);
    vertices = push_vertex(vertices, +extents, +extents, -extents, +0.0f, +0.0f, -1.0f, 0.0f, 1.0f);
    // Top
    vertices = push_vertex(vertices, -extents, +extents, +extents, +0.0f, +1.0f, +0.0f, 0.0f, 0.0f);
    vertices = push_vertex(vertices, +extents, +extents, +extents, +0.0f, +1.0f, +0.0f, 1.0f, 0.0f);
    vertices = push_vertex(vertices, +extents, +extents, -extents, +0.0f, +1.0f, +0.0f, 1.0f, 1.0f);
    vertices = push_vertex(vertices, -extents, +extents, -extents, +0.0f, +1.0f, +0.0f, 0.0f, 1.0f);
    // Bottom
    vertices = push_vertex(vertices, +extents, -extents, -extents, +0.0f, -1.0f, +0.0f, 0.0f, 0.0f);
    vertices = push_vertex(vertices, -extents, -extents, -extents, +0.0f, -1.0f, +0.0f, 1.0f, 0.0f);
    vertices = push_vertex(vertices, -extents, -extents, +extents, +0.0f, -1.0f, +0.0f, 1.0f, 1.0f);
    vertices = push_vertex(vertices, +extents, -extents, +extents, +0.0f, -1.0f, +0.0f, 0.0f, 1.0f);
    // Left
    vertices = push_vertex(vertices, -extents, -extents, -extents, -1.0f, +0.0f, +0.0f, 0.0f, 0.0f);
    vertices = push_vertex(vertices, -extents, -extents, +extents, -1.0f, +0.0f, +0.0f, 1.0f, 0.0f);
    vertices = push_vertex(vertices, -extents, +extents, +extents, -1.0f, +0.0f, +0.0f, 1.0f, 1.0f);
    vertices = push_vertex(vertices, -extents, +extents, -extents, -1.0f, +0.0f, +0.0f, 0.0f, 1.0f);
    // Right
    vertices = push_vertex(vertices, +extents, -extents, +extents, +1.0f, +0.0f, +0.0f, 0.0f, 0.0f);
    vertices = push_vertex(vertices, +extents, -extents, -extents, +1.0f, +0.0f, +0.0f, 1.0f, 0.0f);
    vertices = push_vertex(vertices, +extents, +extents, -extents, +1.0f, +0.0f, +0.0f, 1.0f, 1.0f);
    vertices = push_vertex(vertices, +extents, +extents, +extents, +1.0f, +0.0f, +0.0f, 0.0f, 1.0f);
    return vertices;
}

static GLushort *push_cube_indices(GLushort *indices)
{
    for (GLushort offset = 0; offset < 24; offset = (GLushort)(offset + 4))
    {
        indices = push_quad_indices(indices, offset);
    }
    return indices;
}

void geometry_init_quad(Vertex *vertices, GLushort *indices, float extents)
{
    push_quad_vertices(vertices, extents);
    push_quad_indices(indices, 0);
}

void geometry_init_cube(Vertex *vertices, GLushort *indices, float extents)
{
    push_cube_vertices(vertices, extents);
    push_cube_indices(indices);
}
