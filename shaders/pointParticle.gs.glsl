#version 330 core
//Used to turn a point into 4 triangles, which explode away from each other over time.
layout (points) in;
layout (triangle_strip, max_vertices = 12) out;

in vec3 color[];

out vec3 vcolor;

uniform vec3 fcolor;
uniform float time;

void main(){
	vec4 pos = gl_in[0].gl_Position;
	float time_alive = (1500.0 - time) / 3000.0;// TODO: this is hard coded, change in future
	//top left
	//top right
	//center
	//bottom right
	//bottom left
	//top left again

	vec4 top_left = pos + vec4(-0.01, 0.01, 0.0, 0.0);
	vec4 top_right = pos + vec4(0.01, 0.01, 0.0, 0.0);
	vec4 centre = pos + vec4(0.0, 0.0, 0.0, 0.0);
	vec4 bottom_left = pos + vec4(-0.01, -0.01, 0.0, 0.0);
	vec4 bottom_right = pos + vec4(0.01, -0.01, 0.0, 0.0);

	vec4 vert_displace = vec4(0,0.5,0,0) * pow(time_alive,2);
	vec4 hori_displace = vec4(0.5,0,0,0) * pow(time_alive,2);
	
		vcolor = vec3(0.5,0.5,0.5) * fcolor;
		gl_Position = top_left + vert_displace;
		EmitVertex();

		gl_Position = top_right + vert_displace;
		EmitVertex();

		vcolor = fcolor;
		gl_Position = centre + vert_displace;
		EmitVertex();
	EndPrimitive();

		vcolor = vec3(0.5,0.5,0.5) * fcolor;
		gl_Position = top_right + hori_displace;
		EmitVertex();

		gl_Position = bottom_right + hori_displace;
		EmitVertex();

		vcolor = fcolor;
		gl_Position = centre + hori_displace;
		EmitVertex();
	EndPrimitive();

		vcolor = vec3(0.5,0.5,0.5) * fcolor;
		gl_Position = bottom_right - vert_displace;
		EmitVertex();

		gl_Position = bottom_left - vert_displace;
		EmitVertex();

		vcolor = fcolor;
		gl_Position = centre - vert_displace;
		EmitVertex();
	EndPrimitive();

		vcolor = vec3(0.5,0.5,0.5) * fcolor;
		gl_Position = bottom_left - hori_displace;
		EmitVertex();

		gl_Position = top_left - hori_displace;
		EmitVertex();

		vcolor = fcolor;
		gl_Position = centre - hori_displace;
		EmitVertex();
	EndPrimitive();
	
}