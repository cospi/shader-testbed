#version 450

uniform float u_time;
uniform float u_delta_time;
uniform sampler2D u_texture_0;
uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;
uniform sampler2D u_texture_3;

in vec2 uv;

out vec4 out_color;

void main()
{
    vec2 st = vec2(mod(uv.x + (u_time * 0.2), 1.0), uv.y);
    out_color.r = texture2D(u_texture_0, st).r;
    out_color.g = texture2D(u_texture_1, st).g;
    out_color.b = texture2D(u_texture_2, st).b;
    out_color.a = texture2D(u_texture_3, st).a;
}
