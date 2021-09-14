#ifndef SHADER_TESTBED_FONT_H_
#define SHADER_TESTBED_FONT_H_

#include <stdbool.h>

#include "math.h"
#include "texture.h"

typedef struct Font {
    Texture *texture;
    uint16_t char_width;
    uint16_t char_height;
    Vector2 char_uv_size;
} Font;

bool font_init_from_tga(Font *font, const char *tga_path);
bool font_get_char_uv(const Font *font, char character, Vector2 *uv);

#endif // SHADER_TESTBED_FONT_H_
