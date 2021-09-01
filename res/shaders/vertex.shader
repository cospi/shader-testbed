#version 450

uniform float u_time;
uniform float u_delta_time;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_color;

out vec3 color;

void main()
{
    gl_Position = vec4(in_position, 1.0);
    color = in_color;
    color.r = mod(u_time, 1.0);
}
