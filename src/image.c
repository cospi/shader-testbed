#include "image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned char UNCOMPRESSED_TGA_HEADER[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static uint16_t parse_u16_le(const unsigned char *data)
{
    return (uint16_t)(data[0] | (data[1] << 8));
}

Image *image_create_from_tga(const char *tga_path)
{
    Image *image = NULL;
    unsigned char bytes_per_pixel;
    uint16_t width, height;
    unsigned char *pixels, *pixel;

    if (tga_path == NULL) {
        return NULL;
    }

    FILE *file = fopen(tga_path, "rb");
    if (file == NULL) {
        return NULL;
    }

    unsigned char header[18];
    if (fread(header, sizeof header, 1, file) != 1) {
        goto out_close_file;
    }

    if (memcmp(header, UNCOMPRESSED_TGA_HEADER, sizeof UNCOMPRESSED_TGA_HEADER) != 0) {
        goto out_close_file;
    }

    bytes_per_pixel = header[16] / 8;
    if ((bytes_per_pixel != 3 /* BGR */) && (bytes_per_pixel != 4 /* BGRA */)) {
        goto out_close_file;
    }

    width = parse_u16_le(header + 12);
    height = parse_u16_le(header + 14);

    pixels = malloc((uint32_t)width * height * 4);
    if (pixels == NULL) {
        goto out_close_file;
    }

    pixel = pixels;
    for (uint16_t y = 0; y < height; ++y) {
        for (uint16_t x = 0; x < width; ++x) {
            pixel[3] = 0xFF;
            if (fread(pixel, bytes_per_pixel, 1, file) != 1) {
                free(pixels);
                goto out_close_file;
            }
            // Convert BGRA to RGBA.
            unsigned char tmp = pixel[0];
            pixel[0] = pixel[2];
            pixel[2] = tmp;
            pixel += 4;
        }
    }

    image = malloc(sizeof(Image));
    if (image == NULL) {
        free(pixels);
        goto out_close_file;
    }

    image->width = width;
    image->height = height;
    image->pixels = pixels;

out_close_file:
    fclose(file);
    return image;
}

void image_destroy(Image *image)
{
    if (image != NULL) {
        // If image->pixels is NULL, the free does nothing.
        free(image->pixels);
        free(image);
    }
}
