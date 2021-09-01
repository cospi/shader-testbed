#ifndef SHADER_TESTBED_VERTEX_ATTRIBUTE_DESCRIPTOR_H_
#define SHADER_TESTBED_VERTEX_ATTRIBUTE_DESCRIPTOR_H_

#include <stddef.h>

#include <gl/GL.h>

typedef struct VertexAttributeDescriptor {
    GLuint index;
    GLint dimension;
    GLenum type;
    GLboolean normalized;
    size_t offset;
} VertexAttributeDescriptor;

#endif // SHADER_TESTBED_VERTEX_ATTRIBUTE_DESCRIPTOR_H_
