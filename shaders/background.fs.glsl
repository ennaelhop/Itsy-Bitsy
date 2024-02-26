#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float depth;
uniform vec2 viewPos;
uniform vec2 lightPos;
uniform int level;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	if(level != 1){
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * vec3(1.0f);
	vec3 cam_pos = vec3(viewPos.x, 0.0f, 1.0f);

	vec3 norm = normalize(cam_pos);
	vec3 player_pos = vec3(lightPos.x, lightPos.y, 1.0f);
	vec3 direction = normalize(player_pos);
	float diff = max(dot(norm, direction), 0.0f);
	vec3 diffuse = diff * vec3(1.0f);

	float specularStrength = 0.8;
	vec3 viewDir = normalize (cam_pos - vec3(texcoord, 1.0f));
	vec3 reflectDir = reflect(-direction, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * vec3(1.0);
	vec3 result = (ambient + diffuse + specular) * fcolor;

	color = vec4(result, 1.0) * vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	} else{
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}
}
