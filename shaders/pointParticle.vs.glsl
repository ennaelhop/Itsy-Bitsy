#version 330

// Input attributes
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texcoord;
layout (location = 2) in mat3 transform;


// Passed to fragment shader
out vec2 texcoord;

// Application data
// uniform mat3 transform;
uniform mat3 camera;
uniform mat3 projection;

void main()
{
	texcoord = in_texcoord;

	vec3 pos = projection * camera * (vec3(transform[2].xy + in_position.xy, 1.0));
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}