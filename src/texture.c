#include "texture.h"

static GLuint create_texture(const Image *image)
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return texture;
}

Texture *texture_create_from_image(const Image *image)
{
    if (image == NULL) {
        return NULL;
    }

    Texture *texture = malloc(sizeof(texture));
    if (texture == NULL) {
        return NULL;
    }

    texture->texture = create_texture(image);
    texture->width = image->width;
    texture->height = image->height;
    return texture;
}

Texture *texture_create_from_tga(const char *tga_path)
{
    if (tga_path == NULL) {
        return NULL;
    }

    Image *image = image_create_from_tga(tga_path);
    if (image == NULL) {
        return NULL;
    }

    Texture *texture = texture_create_from_image(image);
    image_destroy(image);
    return texture;
}
