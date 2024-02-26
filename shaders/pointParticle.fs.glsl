#version 330

// From Vertex Shader
in vec3 vcolor;

// Application data
uniform vec3 fcolor;
uniform float time;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	//color = vec4(fcolor * vcolor, 1.0);
	//color = vec4(fcolor, 1.0);
	float alpha = 1.0 * pow(time/750.0,2);
	color = vec4(vcolor, alpha);
	//c3 col = vcolor;///vec3(255,255,255); //im an idiot so here's some division to get in the right colour space
	//color = vec4(col, 1.0);
}