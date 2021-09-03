#version 450

uniform float u_time;
uniform float u_delta_time;
uniform sampler2D u_texture_0;
uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;
uniform sampler2D u_texture_3;
uniform mat4 u_projection;

in vec2 uv;

out vec4 out_color;

void main()
{
    vec4 a = texture2D(u_texture_0, uv);
    vec4 b = texture2D(u_texture_1, uv);
    out_color = mix(a, b, mod(u_time * 0.5, 1.0));
}
