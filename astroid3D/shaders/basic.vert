#version 330

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

out vec2 UV;
out vec3 N;
out vec3 worldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position =  projection * view * model * vec4(vertexPosition_modelspace,1);
    UV = vertexUV;
	N = normalize((transpose(inverse(model)) * vec4(normal, 1.0)).xyz); //calculate inverse transpose model matrix in program itself and then assign to uniform instead
	worldPos = (model * vec4(vertexPosition_modelspace, 1.0)).xyz;
}

