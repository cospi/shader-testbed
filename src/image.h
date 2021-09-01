#ifndef SHADER_TESTBED_IMAGE_H_
#define SHADER_TESTBED_IMAGE_H_

#include <stdint.h>

typedef struct Image {
    uint16_t width;
    uint16_t height;
    unsigned char *pixels;
} Image;

Image *image_create_from_tga(const char *tga_path);
void image_destroy(Image *image);

#endif // SHADER_TESTBED_IMAGE_H_
