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

uniform vec2 textureRes;
uniform vec2 spriteRes;
uniform int frameIndex;

void main()
{
	vec2 ratioPerSprite = spriteRes/textureRes;
	int numPerRow = int(textureRes.x/spriteRes.x);
	vec2 origin = vec2((frameIndex % numPerRow), (frameIndex / numPerRow)) * ratioPerSprite;
	texcoord = origin + (ratioPerSprite*in_texcoord);//in_texcoord;
	//texcoord = ratioPerSprite + in_texcoord;


	vec3 pos = projection * camera * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}