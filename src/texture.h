#ifndef SHADER_TESTBED_TEXTURE_H_
#define SHADER_TESTBED_TEXTURE_H_

#include <stdint.h>

#include <gl/GL.h>

#include "image.h"

typedef struct Texture {
    GLuint texture;
    uint16_t width;
    uint16_t height;
} Texture;

Texture *texture_create_from_image(const Image *image);
Texture *texture_create_from_tga(const char *tga_path);

#endif // SHADER_TESTBED_TEXTURE_H_