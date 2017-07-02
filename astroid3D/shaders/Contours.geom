#version 330
//Geometry shader to update particles on the GPU

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec2 UV[];
in vec3 N[];
in vec3 worldPos[];
in vec3 EyeDirection_cameraspace[];
in vec3 Normal_cameraspace[];

out vec2 UVgout;
out vec3 Ngout;
out vec3 worldPosgout;
out vec3 EyeDirection_cameraspacegout;
out vec3 Normal_cameraspacegout;

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
		EyeDirection_cameraspacegout = EyeDirection_cameraspace[0]; 
		Normal_cameraspacegout = Normal_cameraspace[0];

		EmitVertex();

		UVgout = UV[1];
		Ngout  = N[1];
		worldPosgout = worldPos[1];
		gl_Position = gl_in[1].gl_Position;
		EyeDirection_cameraspacegout = EyeDirection_cameraspace[1];
		Normal_cameraspacegout = Normal_cameraspace[1];

		EmitVertex();

		UVgout = UV[2];
		Ngout  = N[2];
		worldPosgout = worldPos[2];
		gl_Position = gl_in[2].gl_Position;
		EyeDirection_cameraspacegout = EyeDirection_cameraspace[2];
		Normal_cameraspacegout = Normal_cameraspace[2];

		EmitVertex();
		EndPrimitive();
	}
}  