#version 330

// Input attributes
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texcoord;
layout (location = 2) in int in_index;
layout (location = 3) in mat3 in_transform;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 camera;
uniform mat3 projection;

uniform vec2 textureRes;
uniform vec2 spriteRes;

void main()
{
	vec2 ratioPerSprite = spriteRes/textureRes;
	int numPerRow = int(textureRes.x/spriteRes.x);
	vec2 origin = vec2((in_index % numPerRow), (in_index / numPerRow)) * ratioPerSprite;
	texcoord = origin + (ratioPerSprite*in_texcoord);//in_texcoord;
	//texcoord = ratioPerSprite + in_texcoord;


	vec3 pos = projection * in_transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}