#version 450

uniform float u_time;
uniform float u_delta_time;
uniform sampler2D u_texture_0;
uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;
uniform sampler2D u_texture_3;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_uv;

out vec2 uv;

void main()
{
    gl_Position = vec4(in_position, 1.0);
    uv = in_uv;
}
