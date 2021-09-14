#include "sprite_batch.h"

#include <stdlib.h>

#include "gl.h"

#define VERTICES_PER_SPRITE 4
#define INDICES_PER_SPRITE 6

typedef struct Vertex {
    Vector2 position;
    Vector2 uv;
} Vertex;

static Vertex *push_vertex(Vertex *vertices, Vector2 position, Vector2 uv)
{
    vertices->position = position;
    vertices->uv = uv;
    return vertices + 1;
}

static Vertex *push_sprite(Vertex *vertices, const Sprite *sprite)
{
    Vector2 rect_min, rect_max, uv_min, uv_max;
    rect_min_max(&sprite->rect, &rect_min, &rect_max);
    rect_min_max(&sprite->uv, &uv_min, &uv_max);

    vertices = push_vertex(vertices, rect_min, uv_min);
    vertices = push_vertex(vertices, (Vector2) { rect_max.x, rect_min.y }, (Vector2) { uv_max.x, uv_min.y });
    vertices = push_vertex(vertices, rect_max, uv_max);
    vertices = push_vertex(vertices, (Vector2) { rect_min.x, rect_max.y }, (Vector2) { uv_min.x, uv_max.y });
    return vertices;
}

bool sprite_batch_init(SpriteBatch *sprite_batch, size_t sprite_capacity)
{
    if (sprite_batch == NULL) {
        return false;
    }

    size_t indices_size = sprite_capacity * INDICES_PER_SPRITE * sizeof(GLushort);
    GLushort *indices = malloc(indices_size);
    if (indices == NULL) {
        return false;
    }

    GLushort *index = indices;
    for (size_t i = 0; i < sprite_capacity; ++i) {
        size_t offset = i * VERTICES_PER_SPRITE;
        *(index++) = (GLushort)offset;
        *(index++) = (GLushort)(offset + 1);
        *(index++) = (GLushort)(offset + 2);
        *(index++) = (GLushort)offset;
        *(index++) = (GLushort)(offset + 2);
        *(index++) = (GLushort)(offset + 3);
    }

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    GLuint *buffers = sprite_batch->buffers;
    glGenBuffers(2, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(
        GL_ARRAY_BUFFER,
        (GLsizeiptr)(sprite_capacity * VERTICES_PER_SPRITE * sizeof(Vertex)),
        NULL,
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)indices_size, indices, GL_STATIC_DRAW);
    free(indices);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, uv));

    sprite_batch->vertex_array = vertex_array;
    sprite_batch->sprite_capacity = sprite_capacity;
    sprite_batch->sprite_count = 0;
    sprite_batch->mapped_buffer = NULL;
    return true;
}

bool sprite_batch_begin(SpriteBatch *sprite_batch)
{
    if (sprite_batch == NULL) {
        return false;
    }

    glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->buffers[0]);
    void *buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    if (buffer == NULL) {
        return false;
    }

    sprite_batch->mapped_buffer = buffer;
    sprite_batch->sprite_count = 0;
    return true;
}

void sprite_batch_push(SpriteBatch *sprite_batch, const Sprite *sprite)
{
    if (
        (sprite_batch != NULL)
        && (sprite_batch->sprite_count < sprite_batch->sprite_capacity)
        && (sprite_batch->mapped_buffer != NULL)
        && (sprite != NULL)
    ) {
        push_sprite((Vertex *)sprite_batch->mapped_buffer + (sprite_batch->sprite_count * VERTICES_PER_SPRITE), sprite);
        ++sprite_batch->sprite_count;
    }

}

void sprite_batch_end(SpriteBatch *sprite_batch)
{
    if (sprite_batch != NULL) {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        sprite_batch->mapped_buffer = NULL;
    }
}

void sprite_batch_draw(const SpriteBatch *sprite_batch)
{
    if (sprite_batch != NULL) {
        glBindVertexArray(sprite_batch->vertex_array);
        glDrawElements(
            GL_TRIANGLES,
            (GLsizei)(sprite_batch->sprite_count * INDICES_PER_SPRITE),
            GL_UNSIGNED_SHORT,
            NULL
        );
    }
}
