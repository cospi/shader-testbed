#version 450

uniform sampler2D u_texture;

in vec2 uv;

out vec4 out_color;

void main()
{
    out_color = texture(u_texture, uv);
}
