#ifndef SHADER_TESTBED_MATH_H_
#define SHADER_TESTBED_MATH_H_

#include <math.h>
#include <string.h>

#define MATH_PI 3.14159265358979323846f
#define MATH_DEG2RAD (MATH_PI / 180.0f)

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

typedef float Matrix4x4[16];

static inline void matrix4x4_zero(Matrix4x4 matrix)
{
    memset(matrix, 0, sizeof(Matrix4x4));
}

static inline void matrix4x4_identity(Matrix4x4 matrix)
{
    matrix4x4_zero(matrix);
    matrix[0] = 1.0f;
    matrix[5] = 1.0f;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
}

static inline void matrix4x4_translate(Matrix4x4 matrix, Vector3 translation)
{
    matrix4x4_identity(matrix);
    matrix[3] = translation.x;
    matrix[7] = translation.y;
    matrix[11] = translation.z;
}

static inline void matrix4x4_rotate(Matrix4x4 matrix, Vector3 axis, float angle)
{
    float c = cosf(angle);
    float d = 1.0f - c;
    float s = sinf(angle);
    float xx = axis.x * axis.x;
    float xy = axis.x * axis.y;
    float xz = axis.x * axis.z;
    float yy = axis.y * axis.y;
    float yz = axis.y * axis.z;
    float zz = axis.z * axis.z;
    float xs = axis.x * s;
    float ys = axis.y * s;
    float zs = axis.z * s;
    matrix4x4_zero(matrix);
    matrix[0] = (xx * d) + c;
    matrix[1] = (xy * d) - zs;
    matrix[2] = (xz * d) + ys;
    matrix[4] = (xy * d) + zs;
    matrix[5] = (yy * d) + c;
    matrix[6] = (yz * d) - xs;
    matrix[8] = (xz * d) - ys;
    matrix[9] = (yz * d) + xs;
    matrix[10] = (zz * d) + c;
    matrix[15] = 1.0f;
}


static inline void matrix4x4_perspective(
    Matrix4x4 matrix,
    float fov,
    float aspect_ratio,
    float near_plane,
    float far_plane
)
{
    float tan_fov_half_inverse = 1.0f / (tanf(fov * 0.5f));
    float near_minus_far_inverse = 1.0f / (near_plane - far_plane);
    matrix4x4_zero(matrix);
    matrix[0] = tan_fov_half_inverse / aspect_ratio;
    matrix[5] = tan_fov_half_inverse;
    matrix[10] = (near_plane + far_plane) * near_minus_far_inverse;
    matrix[11] = near_plane * far_plane * 2.0f * near_minus_far_inverse;
    matrix[14] = -1.0f;
    matrix[15] = 1.0f;
}

static inline void matrix4x4_multiply(Matrix4x4 result, const Matrix4x4 left, const Matrix4x4 right)
{
    for (size_t y = 0; y < 4; ++y) {
        size_t row_offset = y * 4;
        const float *left_row = left + row_offset;
        float *result_row = result + row_offset;
        for (size_t x = 0; x < 4; ++x) {
            float value = 0.0f;
            for (size_t i = 0; i < 4; ++i) {
                value += left_row[i] * right[(i * 4) + x];
            }
            result_row[x] = value;
        }
    }
}

#endif // SHADER_TESTBED_MATH_H_
