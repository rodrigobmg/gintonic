//
// gp_cdn.fs
//
// Geometry Pass: Color, Diffuse and Normal
//

#version 330

#define GBUFFER_POSITION 0
#define GBUFFER_DIFFUSE 1
#define GBUFFER_NORMAL 2
#define GBUFFER_TEXCOORD 3
#define GBUFFER_COUNT 4

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;

layout (location = GBUFFER_POSITION) out vec3 out_position;
layout (location = GBUFFER_DIFFUSE) out vec4 out_diffuse;
layout (location = GBUFFER_NORMAL) out vec3 out_normal;
layout (location = GBUFFER_TEXCOORD) out vec2 out_texcoord;

uniform struct Material {
	vec4 color;
	sampler2D diffuse;
	sampler2D normal;
} material;

void main()
{
	// TODO!!!

	vec3 matnormal = texture(material.normal, v_texcoord).xyz;
	vec3 surfnormal = normalize(v_normal);
	surfnormal.x += (matnormal.x - 0.5f);
	surfnormal.y += (matnormal.y - 0.5f);
	surfnormal.z += (matnormal.z - 0.5f);

	out_position = v_position;
	out_diffuse  = material.color * texture(material.diffuse, v_texcoord);
	out_normal   = normalize(surfnormal);
	out_texcoord = v_texcoord;
}
