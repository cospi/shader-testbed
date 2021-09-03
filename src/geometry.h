#ifndef SHADER_TESTBED_GEOMETRY_H_
#define SHADER_TESTBED_GEOMETRY_H_

#include <gl/GL.h>

#include "vertex.h"

#define GEOMETRY_QUAD_VERTEX_COUNT 4
#define GEOMETRY_QUAD_INDEX_COUNT 6
#define GEOMETRY_CUBE_VERTEX_COUNT 24
#define GEOMETRY_CUBE_INDEX_COUNT 36

void geometry_init_quad(Vertex *vertices, GLushort *indices, float extents);
void geometry_init_cube(Vertex *vertices, GLushort *indices, float extents);

#endif // SHADER_TESTBED_GEOMETRY_H_
