//
// lp_directional.fs
//
// Uses lp.vs
//

#version 330 core

uniform vec2 viewport_size;

uniform struct GeometryBuffer
{
	sampler2D position;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
} gbuffer;

uniform struct DirectionalLight {
	vec4 intensity;
	vec3 direction;
} light;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculate_screen_position()
{
	return vec2(gl_FragCoord.x / viewport_size.x, gl_FragCoord.y / viewport_size.y);
}

out vec3 final_color;

void main()
{
	vec2 screen_position = calculate_screen_position();

	vec3 position = texture(gbuffer.position, screen_position).xyz;
	vec4 diffuse  = texture(gbuffer.diffuse,  screen_position);
	vec4 specular = texture(gbuffer.specular, screen_position);
	vec3 N        = texture(gbuffer.normal,   screen_position).xyz;

	vec3 L = normalize(-light.direction);
	float lambertian = max(dot(L, N), 0.0f);
	vec3 E = normalize(-position);
 	vec3 R = normalize(-reflect(L,N));
	float spec_factor = pow(max(dot(R,E), 0.0f), specular.a);
	spec_factor = clamp(spec_factor, 0.0f, 1.0f); 
	float diffuse_factor = diffuse.a;
	float ambient_factor = 1.0f - diffuse_factor;

	final_color = light.intensity.a * diffuse_factor * lambertian * light.intensity.rgb * diffuse.rgb 
		// + ambient_factor * diffuse.rgb 
		+ spec_factor * specular.rgb * light.intensity.rgb;
		// + spec_factor * vec3(1.0f, 0.0f, 0.0f);
}