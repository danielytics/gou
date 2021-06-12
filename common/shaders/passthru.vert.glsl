#version 450 core

// layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
// layout (location = 1) in vec3 aNormal;
// layout (location = 2) in vec2 aUV;
  
// out vec4 vertexColor; // specify a color output to the fragment shader

// void main()
// {
//     gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
//     vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color
// }

layout (location = 0) in vec3 in_position;
// layout (location = 1) in vec3 in_normal;
// layout (location = 2) in vec3 in_texture_coords;

layout (std140) uniform Matrices
{
	mat4 projection_view;
};

uniform mat4 u_model;

void main()
{
    gl_Position = projection_view * u_model * vec4(in_position, 1.0);
}