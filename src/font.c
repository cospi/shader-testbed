#include "font.h"

#define CHARS_PER_ROW 16
#define CHARS_PER_COL 16

bool font_init_from_tga(Font *font, const char *tga_path)
{
    if ((font == NULL) || (tga_path == NULL)) {
        return false;
    }

    Texture *texture = texture_create_from_tga(tga_path, GL_NEAREST);
    if (texture == NULL) {
        return false;
    }

    font->texture = texture;
    font->char_width = texture->width / CHARS_PER_ROW;
    font->char_height = texture->height / CHARS_PER_COL;
    font->char_uv_size.x = (float)font->char_width / texture->width;
    font->char_uv_size.y = (float)font->char_height / texture->height;
    return true;
}

bool font_get_char_uv(const Font *font, char character, Vector2 *uv)
{
    if ((font == NULL) || (uv == NULL)) {
        return false;
    }

    const Texture *texture = font->texture;
    if (texture == NULL) {
        return false;
    }

    int row = character / CHARS_PER_ROW;
    int col = character % CHARS_PER_ROW;
    uv->x = (float)(col * font->char_width) / texture->width;
    uv->y = (float)((CHARS_PER_COL - row - 1) * font->char_height) / texture->height;
    return true;
}
