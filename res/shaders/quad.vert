#version 450

uniform float u_time;
uniform float u_delta_time;
uniform sampler2D u_texture_0;
uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;
uniform sampler2D u_texture_3;
uniform mat4 u_transform;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out vec2 uv;

void main()
{
    gl_Position = u_transform * vec4(in_position, 1.0);
    uv = in_uv;
}
