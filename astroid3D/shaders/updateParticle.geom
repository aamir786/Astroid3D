#version 330
//Geometry shader to update particles on the GPU

layout(points) in;
layout(points, max_vertices=1) out;

in vec3 inVel[];
in float inTime[];

out vec3 outPos;
out vec3 outVel;
out float outTime;

uniform float dTime;

const float maxTime=3000;

void main(){
	outTime=inTime[0]+dTime;
	if(outTime<maxTime){
		outPos=gl_in[0].gl_Position.xyz+inVel[0]*inTime[0];
		outVel=inVel[0];
		EmitVertex();
		EndPrimitive();
	}
}