#version 330

layout(location = 0)in vec3 pos;
layout(location = 1)in vec3 vel;
layout(location = 2)in float time;

out vec3 inVel;
out float inTime;

void main(){
	gl_Position=vec4(pos, 1.0);
	inTime=time;
	inVel=vel;
}