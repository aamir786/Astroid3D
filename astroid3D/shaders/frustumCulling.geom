#version 330
//Geometry shader

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec2 UV[];
in vec3 N[];
in vec3 worldPos[];

out vec2 UVgout;
out vec3 Ngout;
out vec3 worldPosgout;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

bool visible(vec4 p)
{
    return !(( p.x < -(p.w)) || ( p.x >  (p.w))||
             ( p.y < -(p.w)) || ( p.y >  (p.w))||
             ( p.z < -(p.w)) || ( p.z >  (p.w)));
}

void main()
{
	if(visible(gl_in[0].gl_Position) || visible(gl_in[1].gl_Position) || visible(gl_in[2].gl_Position)) {
		UVgout = UV[0];
		Ngout  = N[0];
		worldPosgout = worldPos[0];
		gl_Position = gl_in[0].gl_Position; 
		EmitVertex();

		UVgout = UV[1];
		Ngout  = N[1];
		worldPosgout = worldPos[1];
		gl_Position = gl_in[1].gl_Position;
		EmitVertex();

		UVgout = UV[2];
		Ngout  = N[2];
		worldPosgout = worldPos[2];
		gl_Position = gl_in[2].gl_Position;
		EmitVertex();
		EndPrimitive();
	}
}  