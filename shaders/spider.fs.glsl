#version 330

// From vertex shader
in vec2 texcoord;
uniform float time;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int glow;

// Output color
layout(location = 0) out  vec4 color;

void main() {

	// glow effect: https://www.shadertoy.com/view/lsXGWn

	const float intensity = 1.5;

	vec4 sum = texture(sampler0, vec2(texcoord.x, texcoord.y)) * 0.5; // don't want it to be fully white (1.0)

	// if glow is activated, change spider color to glow
	if (glow == 1) {
		if(sin(time) > 0.0) { 
			color = sum * sin(time)*intensity+ texture(sampler0, vec2(texcoord.x, texcoord.y));
		}
		else {
			color = sum * -sin(time)*intensity+ texture(sampler0, vec2(texcoord.x, texcoord.y));
		}
	} else {
		// regular/default spider color 
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}

}