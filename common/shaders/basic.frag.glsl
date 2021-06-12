#version 450 core

struct Material {
    float albedo;
};

in vec2 in_tex_coords;

out vec4 out_color;

uniform Material material;
uniform sampler2DArray textures_albedo;

void main()
{
	out_color = texture(texture_albedo, vec3(in_tex_coords, material.albedo));
}