//
// directional_diffuse.vs
//

#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

uniform mat4 matrix_PVM;
uniform mat3 matrix_N;

out vec2 v_uv;
out vec3 v_normal;

void main()
{
	gl_Position = matrix_PVM * vec4(in_position, 1.0f);
	v_uv = in_uv;
	v_normal = matrix_N * in_normal;
}