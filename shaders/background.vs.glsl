#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 camera;
uniform mat3 projection;
uniform float depth;

void main()
{
	// camera[2].xy tells us the camera's position
	// dividing by -depth + 1 gives us an appropriate scaling factor for the layers. IF DEPTH > 0 THIS MIGHT NOT WORK PROPERLY
	// transform[0][0] is used to convert between our screen coordinates on [0, screen_width] and the tex coordinates on [0,1]
	texcoord = in_texcoord - camera[2].xy / ((-depth + 1) * transform[0][0]) ;  //adjust texture for parallax

	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}