#include "text.h"

void text_push(
    SpriteBatch *sprite_batch,
    const Font *font,
    const char *text,
    Vector2 position,
    float char_scale,
    float newline_offset
)
{
    if ((sprite_batch == NULL) || (font == NULL) || (text == NULL)) {
        return;
    }

    Sprite sprite;
    sprite.rect.size.x = font->char_width * char_scale;
    sprite.rect.size.y = font->char_height * char_scale;
    sprite.rect.position.x = position.x;
    sprite.rect.position.y = position.y - sprite.rect.size.y;
    sprite.uv.size = font->char_uv_size;

    char character;
    while ((character = *(text++)) != '\0') {
        if (character == '\n') {
            sprite.rect.position.x = position.x;
            sprite.rect.position.y -= sprite.rect.size.y + newline_offset;
        } else {
            if (font_get_char_uv(font, character, &sprite.uv.position)) {
                sprite_batch_push(sprite_batch, &sprite);
            }
            sprite.rect.position.x += sprite.rect.size.x;
        }
    }
}
