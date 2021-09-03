#ifndef SHADER_TESTBED_TEXTURE_H_
#define SHADER_TESTBED_TEXTURE_H_

#include <stdint.h>

#include <GL/gl.h>

#include "image.h"

typedef struct Texture {
    GLuint texture;
    uint16_t width;
    uint16_t height;
} Texture;

Texture *texture_create_from_image(const Image *image);
Texture *texture_create_from_tga(const char *tga_path);
void texture_destroy(Texture *texture);

#endif // SHADER_TESTBED_TEXTURE_H_
