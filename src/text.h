#ifndef SHADER_TESTBED_TEXT_H_
#define SHADER_TESTBED_TEXT_H_

#include "font.h"
#include "sprite_batch.h"

void text_push(
    SpriteBatch *sprite_batch,
    const Font *font,
    const char *text,
    Vector2 position,
    float char_scale,
    float newline_offset
);

#endif // SHADER_TESTBED_TEXT_H_
