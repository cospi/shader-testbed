#version 450

uniform float u_time;
uniform float u_delta_time;

layout (location = 0) in vec3 color;

out vec4 out_color;

void main()
{
    out_color = vec4(color, 1.0);
}
