#include "texture.h"

#include <stddef.h>
#include <stdlib.h>

static GLuint create_texture(const Image *image, GLint filter)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        (GLsizei)image->width,
        (GLsizei)image->height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image->pixels
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return texture;
}

Texture *texture_create_from_image(const Image *image, GLint filter)
{
    if (image == NULL) {
        return NULL;
    }

    Texture *texture = malloc(sizeof(Texture));
    if (texture == NULL) {
        return NULL;
    }

    texture->texture = create_texture(image, filter);
    texture->width = image->width;
    texture->height = image->height;
    return texture;
}

Texture *texture_create_from_tga(const char *tga_path, GLint filter)
{
    if (tga_path == NULL) {
        return NULL;
    }

    Image *image = image_create_from_tga(tga_path);
    if (image == NULL) {
        return NULL;
    }

    Texture *texture = texture_create_from_image(image, filter);
    image_destroy(image);
    return texture;
}

void texture_destroy(Texture *texture)
{
    if (texture != NULL) {
        glDeleteTextures(1, &texture->texture);
        free(texture);
    }
}
