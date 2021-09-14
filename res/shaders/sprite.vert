#version 450

uniform mat4 u_projection;

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_uv;

out vec2 uv;

void main()
{
    gl_Position = u_projection * vec4(in_position, 0.0, 1.0);
    uv = in_uv;
}
