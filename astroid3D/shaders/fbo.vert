#version 330

layout(location = 0) in vec3 position;

out vec2 UV;

void main(){
	//quad vertices are in [-1; 1], we need [0, 1]
	gl_Position=vec4(position, 1.0);
	UV=(vec2(1.0, 1.0)+position.xy)/2.0;
}