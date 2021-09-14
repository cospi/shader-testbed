#ifndef SHADER_TESTBED_SPRITE_BATCH_H_
#define SHADER_TESTBED_SPRITE_BATCH_H_

#include <stdbool.h>
#include <stddef.h>

#include <GL/gl.h>

#include "sprite.h"

typedef struct SpriteBatch {
    size_t sprite_capacity;
    size_t sprite_count;
    GLuint vertex_array;
    GLuint buffers[2];
    void *mapped_buffer;
} SpriteBatch;

bool sprite_batch_init(SpriteBatch *sprite_batch, size_t sprite_capacity);
bool sprite_batch_begin(SpriteBatch *sprite_batch);
void sprite_batch_push(SpriteBatch *sprite_batch, const Sprite *sprite);
void sprite_batch_end(SpriteBatch *sprite_batch);
void sprite_batch_draw(const SpriteBatch *sprite_batch);

#endif // SHADER_TESTBED_SPRITE_BATCH_H_
